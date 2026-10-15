/* io.cpp */

#include "io.hpp"

#include <algorithm>
#include <cerrno>
#include <climits>
#include <limits>
#include <system_error>
#include <utility>

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#else

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>

#endif

namespace optic::io {

namespace {

/* ============================================================
 * Error handling
 * ============================================================ */

thread_local IOStatus current_status = IOStatus::Success;
thread_local int current_system_error = 0;

void set_error(IOStatus status, int system_error = 0) noexcept {
    current_status = status;
    current_system_error = system_error;
}

IOStatus map_errno(int error) noexcept {
    switch (error) {
        case 0:
            return IOStatus::Success;

        case EACCES:
        case EPERM:
            return IOStatus::PermissionDenied;

        case ENOENT:
            return IOStatus::NotFound;

        case EEXIST:
            return IOStatus::AlreadyExists;

        case ENOMEM:
            return IOStatus::OutOfMemory;

        case EINVAL:
            return IOStatus::InvalidArgument;

#ifdef EAGAIN
        case EAGAIN:
            return IOStatus::WouldBlock;
#endif

#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
            return IOStatus::WouldBlock;
#endif

        case EINTR:
            return IOStatus::Interrupted;

        default:
            return IOStatus::SystemError;
    }
}

#ifdef _WIN32

IOStatus map_windows_error(DWORD error) noexcept {
    switch (error) {
        case ERROR_SUCCESS:
            return IOStatus::Success;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            return IOStatus::NotFound;

        case ERROR_ACCESS_DENIED:
            return IOStatus::PermissionDenied;

        case ERROR_FILE_EXISTS:
        case ERROR_ALREADY_EXISTS:
            return IOStatus::AlreadyExists;

        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            return IOStatus::OutOfMemory;

        case ERROR_INVALID_HANDLE:
            return IOStatus::InvalidStream;

        case ERROR_INVALID_PARAMETER:
            return IOStatus::InvalidArgument;

        case ERROR_BROKEN_PIPE:
        case ERROR_HANDLE_EOF:
            return IOStatus::EndOfFile;

        case ERROR_IO_PENDING:
            return IOStatus::WouldBlock;

        default:
            return IOStatus::SystemError;
    }
}

void set_windows_error(DWORD error) noexcept {
    set_error(map_windows_error(error), static_cast<int>(error));
}

#endif

IOResult make_result(
    IOStatus status,
    std::size_t bytes = 0
) noexcept {
    set_error(status,
              status == IOStatus::Success ? 0 : current_system_error);

    return {status, bytes};
}

} // namespace

/* ============================================================
 * Stream backend
 * ============================================================ */

struct Stream::Impl {
    StreamType stream_type = StreamType::Custom;

    bool readable = false;
    bool writable = false;
    bool seekable = false;
    bool terminal = false;
    bool owned = false;

    BufferMode buffer_mode = BufferMode::None;

#ifdef _WIN32

    HANDLE handle = INVALID_HANDLE_VALUE;

#else

    int fd = -1;

#endif
};

/* ============================================================
 * Stream lifecycle
 * ============================================================ */

Stream::Stream() noexcept = default;

Stream::Stream(std::unique_ptr<Impl> impl) noexcept
    : impl_(std::move(impl)) {}

Stream::~Stream() {
    close();
}

Stream::Stream(Stream&& other) noexcept = default;

Stream& Stream::operator=(Stream&& other) noexcept = default;

bool Stream::is_open() const noexcept {
    if (!impl_) {
        return false;
    }

#ifdef _WIN32
    return impl_->handle != nullptr &&
           impl_->handle != INVALID_HANDLE_VALUE;
#else
    return impl_->fd >= 0;
#endif
}

bool Stream::is_readable() const noexcept {
    return is_open() && impl_->readable;
}

bool Stream::is_writable() const noexcept {
    return is_open() && impl_->writable;
}

bool Stream::is_terminal() const noexcept {
    return is_open() && impl_->terminal;
}

bool Stream::is_seekable() const noexcept {
    return is_open() && impl_->seekable;
}

StreamType Stream::type() const noexcept {
    return impl_ ? impl_->stream_type : StreamType::Custom;
}

/* ============================================================
 * Standard streams
 * ============================================================ */

Stream& standard_input() noexcept {
    static Stream stream([] {
        auto impl = std::make_unique<Impl>();

        impl->stream_type = StreamType::StandardInput;
        impl->readable = true;
        impl->owned = false;

#ifdef _WIN32
        impl->handle = GetStdHandle(STD_INPUT_HANDLE);

        DWORD mode = 0;
        impl->terminal =
            impl->handle != INVALID_HANDLE_VALUE &&
            impl->handle != nullptr &&
            GetConsoleMode(impl->handle, &mode);

#else
        impl->fd = STDIN_FILENO;
        impl->terminal = ::isatty(STDIN_FILENO) == 1;
#endif

        return Stream(std::move(impl));
    }());

    return stream;
}

Stream& standard_output() noexcept {
    static Stream stream([] {
        auto impl = std::make_unique<Impl>();

        impl->stream_type = StreamType::StandardOutput;
        impl->writable = true;
        impl->owned = false;

#ifdef _WIN32
        impl->handle = GetStdHandle(STD_OUTPUT_HANDLE);

        DWORD mode = 0;
        impl->terminal =
            impl->handle != INVALID_HANDLE_VALUE &&
            impl->handle != nullptr &&
            GetConsoleMode(impl->handle, &mode);

#else
        impl->fd = STDOUT_FILENO;
        impl->terminal = ::isatty(STDOUT_FILENO) == 1;
#endif

        return Stream(std::move(impl));
    }());

    return stream;
}

Stream& standard_error() noexcept {
    static Stream stream([] {
        auto impl = std::make_unique<Impl>();

        impl->stream_type = StreamType::StandardError;
        impl->writable = true;
        impl->owned = false;

#ifdef _WIN32
        impl->handle = GetStdHandle(STD_ERROR_HANDLE);

        DWORD mode = 0;
        impl->terminal =
            impl->handle != INVALID_HANDLE_VALUE &&
            impl->handle != nullptr &&
            GetConsoleMode(impl->handle, &mode);

#else
        impl->fd = STDERR_FILENO;
        impl->terminal = ::isatty(STDERR_FILENO) == 1;
#endif

        return Stream(std::move(impl));
    }());

    return stream;
}

/* ============================================================
 * Read / write
 * ============================================================ */

IOResult Stream::read(
    void* buffer,
    std::size_t size
) noexcept {
    if (!is_open()) {
        set_error(IOStatus::InvalidStream);
        return {IOStatus::InvalidStream, 0};
    }

    if (!impl_->readable || (!buffer && size != 0)) {
        set_error(IOStatus::InvalidArgument);
        return {IOStatus::InvalidArgument, 0};
    }

    if (size == 0) {
        set_error(IOStatus::Success);
        return {IOStatus::Success, 0};
    }

#ifdef _WIN32

    const DWORD amount = static_cast<DWORD>(
        std::min(size, static_cast<std::size_t>(MAXDWORD))
    );

    DWORD bytes_read = 0;

    if (!ReadFile(
            impl_->handle,
            buffer,
            amount,
            &bytes_read,
            nullptr)) {
        const DWORD error = GetLastError();

        if (error == ERROR_HANDLE_EOF ||
            error == ERROR_BROKEN_PIPE) {
            set_error(IOStatus::EndOfFile,
                      static_cast<int>(error));
            return {IOStatus::EndOfFile, 0};
        }

        set_windows_error(error);
        return {current_status, 0};
    }

    if (bytes_read == 0) {
        set_error(IOStatus::EndOfFile);
        return {IOStatus::EndOfFile, 0};
    }

    set_error(IOStatus::Success);
    return {IOStatus::Success,
            static_cast<std::size_t>(bytes_read)};

#else

    const std::size_t amount = std::min(
        size,
        static_cast<std::size_t>(
            std::numeric_limits<ssize_t>::max()
        )
    );

    const ssize_t result = ::read(impl_->fd, buffer, amount);

    if (result < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return {current_status, 0};
    }

    if (result == 0) {
        set_error(IOStatus::EndOfFile);
        return {IOStatus::EndOfFile, 0};
    }

    set_error(IOStatus::Success);
    return {IOStatus::Success,
            static_cast<std::size_t>(result)};

#endif
}

IOResult Stream::write(
    const void* buffer,
    std::size_t size
) noexcept {
    if (!is_open()) {
        set_error(IOStatus::InvalidStream);
        return {IOStatus::InvalidStream, 0};
    }

    if (!impl_->writable || (!buffer && size != 0)) {
        set_error(IOStatus::InvalidArgument);
        return {IOStatus::InvalidArgument, 0};
    }

    if (size == 0) {
        set_error(IOStatus::Success);
        return {IOStatus::Success, 0};
    }

#ifdef _WIN32

    const DWORD amount = static_cast<DWORD>(
        std::min(size, static_cast<std::size_t>(MAXDWORD))
    );

    DWORD bytes_written = 0;

    if (!WriteFile(
            impl_->handle,
            buffer,
            amount,
            &bytes_written,
            nullptr)) {
        set_windows_error(GetLastError());
        return {current_status, 0};
    }

    set_error(IOStatus::Success);
    return {IOStatus::Success,
            static_cast<std::size_t>(bytes_written)};

#else

    const std::size_t amount = std::min(
        size,
        static_cast<std::size_t>(
            std::numeric_limits<ssize_t>::max()
        )
    );

    const ssize_t result = ::write(impl_->fd, buffer, amount);

    if (result < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return {current_status, 0};
    }

    set_error(IOStatus::Success);
    return {IOStatus::Success,
            static_cast<std::size_t>(result)};

#endif
}

/* ============================================================
 * Flush / close
 * ============================================================ */

IOStatus Stream::flush() noexcept {
    if (!is_open()) {
        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

    /*
     * These backends use native OS I/O, not a user-space
     * output buffer. There is currently nothing to flush.
     *
     * This does not guarantee durable storage on disk.
     */

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus Stream::close() noexcept {
    if (!impl_) {
        return IOStatus::Success;
    }

    if (!is_open()) {
        impl_.reset();
        return IOStatus::Success;
    }

    /*
     * Standard streams are borrowed resources.
     * Never close the process's standard descriptors here.
     */

    if (!impl_->owned) {
        if (impl_->stream_type == StreamType::StandardInput ||
            impl_->stream_type == StreamType::StandardOutput ||
            impl_->stream_type == StreamType::StandardError) {
            return IOStatus::Success;
        }

        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

#ifdef _WIN32

    if (!CloseHandle(impl_->handle)) {
        set_windows_error(GetLastError());
        return current_status;
    }

    impl_->handle = INVALID_HANDLE_VALUE;

#else

    if (::close(impl_->fd) < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

    impl_->fd = -1;

#endif

    impl_.reset();

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

/* ============================================================
 * Seek / tell
 * ============================================================ */

IOStatus Stream::seek(
    std::int64_t offset,
    SeekOrigin origin
) noexcept {
    if (!is_open() || !impl_->seekable) {
        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

#ifdef _WIN32

    LARGE_INTEGER distance;
    distance.QuadPart = offset;

    DWORD method = FILE_BEGIN;

    switch (origin) {
        case SeekOrigin::Begin:
            method = FILE_BEGIN;
            break;

        case SeekOrigin::Current:
            method = FILE_CURRENT;
            break;

        case SeekOrigin::End:
            method = FILE_END;
            break;
    }

    if (!SetFilePointerEx(
            impl_->handle,
            distance,
            nullptr,
            method)) {
        set_windows_error(GetLastError());
        return current_status;
    }

#else

    int whence = SEEK_SET;

    switch (origin) {
        case SeekOrigin::Begin:
            whence = SEEK_SET;
            break;

        case SeekOrigin::Current:
            whence = SEEK_CUR;
            break;

        case SeekOrigin::End:
            whence = SEEK_END;
            break;
    }

    if (::lseek(
            impl_->fd,
            static_cast<off_t>(offset),
            whence) == static_cast<off_t>(-1)) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

#endif

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus Stream::tell(std::int64_t* position) noexcept {
    if (!position || !is_open() || !impl_->seekable) {
        set_error(IOStatus::InvalidArgument);
        return IOStatus::InvalidArgument;
    }

#ifdef _WIN32

    LARGE_INTEGER distance{};
    LARGE_INTEGER result{};

    if (!SetFilePointerEx(
            impl_->handle,
            distance,
            &result,
            FILE_CURRENT)) {
        set_windows_error(GetLastError());
        return current_status;
    }

    *position = result.QuadPart;

#else

    const off_t result = ::lseek(impl_->fd, 0, SEEK_CUR);

    if (result == static_cast<off_t>(-1)) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

    *position = static_cast<std::int64_t>(result);

#endif

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus Stream::get_info(StreamInfo* info) const noexcept {
    if (!info || !is_open()) {
        set_error(IOStatus::InvalidArgument);
        return IOStatus::InvalidArgument;
    }

    info->type = impl_->stream_type;
    info->readable = impl_->readable;
    info->writable = impl_->writable;
    info->seekable = impl_->seekable;
    info->terminal = impl_->terminal;
    info->owned = impl_->owned;

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

/* ============================================================
 * File operations
 * ============================================================ */

IOStatus open_file(
    Stream& stream,
    const std::filesystem::path& path,
    OpenMode mode
) noexcept {
    if (path.empty()) {
        set_error(IOStatus::InvalidArgument);
        return IOStatus::InvalidArgument;
    }

    if (stream.is_open()) {
        const IOStatus status = stream.close();

        if (status != IOStatus::Success) {
            return status;
        }
    }

    auto impl = std::make_unique<Stream::Impl>();

    impl->stream_type = StreamType::File;
    impl->owned = true;
    impl->seekable = true;

#ifdef _WIN32

    DWORD access = 0;
    DWORD creation = OPEN_EXISTING;

    switch (mode) {
        case OpenMode::Read:
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            impl->readable = true;
            break;

        case OpenMode::Write:
            access = GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            impl->writable = true;
            break;

        case OpenMode::Append:
            access = FILE_APPEND_DATA;
            creation = OPEN_ALWAYS;
            impl->writable = true;
            break;

        case OpenMode::ReadWrite:
            access = GENERIC_READ | GENERIC_WRITE;
            creation = OPEN_EXISTING;
            impl->readable = true;
            impl->writable = true;
            break;

        case OpenMode::WriteRead:
            access = GENERIC_READ | GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            impl->readable = true;
            impl->writable = true;
            break;

        case OpenMode::AppendRead:
            access = GENERIC_READ | FILE_APPEND_DATA;
            creation = OPEN_ALWAYS;
            impl->readable = true;
            impl->writable = true;
            break;
    }

    impl->handle = CreateFileW(
        path.c_str(),
        access,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        creation,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (impl->handle == INVALID_HANDLE_VALUE) {
        set_windows_error(GetLastError());
        return current_status;
    }

    if (mode == OpenMode::Append ||
        mode == OpenMode::AppendRead) {
        LARGE_INTEGER zero{};

        if (!SetFilePointerEx(
                impl->handle,
                zero,
                nullptr,
                FILE_END)) {
            const DWORD error = GetLastError();
            CloseHandle(impl->handle);
            set_windows_error(error);
            return current_status;
        }
    }

#else

    int flags = 0;

    switch (mode) {
        case OpenMode::Read:
            flags = O_RDONLY;
            impl->readable = true;
            break;

        case OpenMode::Write:
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            impl->writable = true;
            break;

        case OpenMode::Append:
            flags = O_WRONLY | O_CREAT | O_APPEND;
            impl->writable = true;
            break;

        case OpenMode::ReadWrite:
            flags = O_RDWR;
            impl->readable = true;
            impl->writable = true;
            break;

        case OpenMode::WriteRead:
            flags = O_RDWR | O_CREAT | O_TRUNC;
            impl->readable = true;
            impl->writable = true;
            break;

        case OpenMode::AppendRead:
            flags = O_RDWR | O_CREAT | O_APPEND;
            impl->readable = true;
            impl->writable = true;
            break;
    }

    impl->fd = ::open(path.c_str(), flags, 0666);

    if (impl->fd < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

#endif

    stream = Stream(std::move(impl));

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

bool file_exists(const std::filesystem::path& path) noexcept {
    std::error_code error;
    const bool exists = std::filesystem::exists(path, error);

    if (error) {
        set_error(IOStatus::SystemError, error.value());
        return false;
    }

    set_error(IOStatus::Success);
    return exists;
}

IOStatus remove_file(
    const std::filesystem::path& path
) noexcept {
    std::error_code error;
    const bool removed = std::filesystem::remove(path, error);

    if (error) {
        set_error(IOStatus::SystemError, error.value());
        return IOStatus::SystemError;
    }

    if (!removed) {
        set_error(IOStatus::NotFound);
        return IOStatus::NotFound;
    }

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus rename_file(
    const std::filesystem::path& old_path,
    const std::filesystem::path& new_path
) noexcept {
    std::error_code error;
    std::filesystem::rename(old_path, new_path, error);

    if (error) {
        set_error(IOStatus::SystemError, error.value());
        return IOStatus::SystemError;
    }

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

/* ============================================================
 * Free-function I/O wrappers
 * ============================================================ */

IOResult read(
    Stream& stream,
    void* buffer,
    std::size_t size
) noexcept {
    return stream.read(buffer, size);
}

IOResult write(
    Stream& stream,
    const void* buffer,
    std::size_t size
) noexcept {
    return stream.write(buffer, size);
}

IOResult read_exact(
    Stream& stream,
    void* buffer,
    std::size_t size
) noexcept {
    if (!buffer && size != 0) {
        set_error(IOStatus::InvalidArgument);
        return {IOStatus::InvalidArgument, 0};
    }

    auto* output = static_cast<unsigned char*>(buffer);
    std::size_t total = 0;

    while (total < size) {
        IOResult result = stream.read(
            output + total,
            size - total
        );

        total += result.bytes_transferred;

        if (result.status != IOStatus::Success) {
            set_error(result.status, current_system_error);
            return {result.status, total};
        }

        if (result.bytes_transferred == 0) {
            set_error(IOStatus::EndOfFile);
            return {IOStatus::EndOfFile, total};
        }
    }

    set_error(IOStatus::Success);
    return {IOStatus::Success, total};
}

IOResult write_all(
    Stream& stream,
    const void* buffer,
    std::size_t size
) noexcept {
    if (!buffer && size != 0) {
        set_error(IOStatus::InvalidArgument);
        return {IOStatus::InvalidArgument, 0};
    }

    const auto* input = static_cast<const unsigned char*>(buffer);
    std::size_t total = 0;

    while (total < size) {
        IOResult result = stream.write(
            input + total,
            size - total
        );

        total += result.bytes_transferred;

        if (result.status != IOStatus::Success) {
            set_error(result.status, current_system_error);
            return {result.status, total};
        }

        if (result.bytes_transferred == 0) {
            set_error(IOStatus::SystemError);
            return {IOStatus::SystemError, total};
        }
    }

    set_error(IOStatus::Success);
    return {IOStatus::Success, total};
}

/* ============================================================
 * Text I/O
 * ============================================================ */

IOStatus read_line(
    Stream& stream,
    std::string& output
) noexcept {
    output.clear();

    char character = '\0';

    for (;;) {
        IOResult result = stream.read(&character, 1);

        if (result.status == IOStatus::EndOfFile) {
            return output.empty()
                ? IOStatus::EndOfFile
                : IOStatus::Success;
        }

        if (result.status != IOStatus::Success) {
            return result.status;
        }

        if (character == '\n') {
            break;
        }

        if (character != '\r') {
            try {
                output.push_back(character);
            } catch (...) {
                set_error(IOStatus::OutOfMemory);
                return IOStatus::OutOfMemory;
            }
        }
    }

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus read_text(
    Stream& stream,
    std::string& output,
    std::size_t max_size
) noexcept {
    output.clear();

    constexpr std::size_t chunk_size = 4096;
    char buffer[chunk_size];

    try {
        while (output.size() < max_size) {
            const std::size_t amount = std::min(
                chunk_size,
                max_size - output.size()
            );

            IOResult result = stream.read(buffer, amount);

            if (result.bytes_transferred > 0) {
                output.append(
                    buffer,
                    result.bytes_transferred
                );
            }

            if (result.status == IOStatus::EndOfFile) {
                set_error(IOStatus::Success);
                return IOStatus::Success;
            }

            if (result.status != IOStatus::Success) {
                return result.status;
            }

            if (result.bytes_transferred == 0) {
                break;
            }
        }
    } catch (...) {
        set_error(IOStatus::OutOfMemory);
        return IOStatus::OutOfMemory;
    }

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOResult write_text(
    Stream& stream,
    std::string_view text
) noexcept {
    return write_all(stream, text.data(), text.size());
}

IOResult write_line(
    Stream& stream,
    std::string_view text
) noexcept {
    IOResult result = write_text(stream, text);

    if (result.status != IOStatus::Success) {
        return result;
    }

    IOResult newline = write_all(stream, "\n", 1);

    return {
        newline.status,
        result.bytes_transferred + newline.bytes_transferred
    };
}

/* ============================================================
 * Buffering
 * ============================================================ */

IOStatus set_buffer_mode(
    Stream& stream,
    BufferMode mode
) noexcept {
    if (!stream.is_open()) {
        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

    /*
     * The current implementation uses direct OS I/O.
     * User-space line/full buffering is not implemented yet.
     */

    if (mode != BufferMode::None) {
        set_error(IOStatus::NotSupported);
        return IOStatus::NotSupported;
    }

    stream.impl_->buffer_mode = mode;

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus sync_stream(Stream& stream) noexcept {
    return stream.flush();
}

/* ============================================================
 * Standard stream redirection
 * ============================================================ */

IOStatus redirect_standard_input(Stream& source) noexcept {
    if (!source.is_open() || !source.is_readable()) {
        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

#ifdef _WIN32

    if (!SetStdHandle(STD_INPUT_HANDLE, source.impl_->handle)) {
        set_windows_error(GetLastError());
        return current_status;
    }

#else

    if (::dup2(source.impl_->fd, STDIN_FILENO) < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

#endif

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus redirect_standard_output(Stream& destination) noexcept {
    if (!destination.is_open() || !destination.is_writable()) {
        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

#ifdef _WIN32

    if (!SetStdHandle(STD_OUTPUT_HANDLE, destination.impl_->handle)) {
        set_windows_error(GetLastError());
        return current_status;
    }

#else

    if (::dup2(destination.impl_->fd, STDOUT_FILENO) < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

#endif

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus redirect_standard_error(Stream& destination) noexcept {
    if (!destination.is_open() || !destination.is_writable()) {
        set_error(IOStatus::InvalidStream);
        return IOStatus::InvalidStream;
    }

#ifdef _WIN32

    if (!SetStdHandle(STD_ERROR_HANDLE, destination.impl_->handle)) {
        set_windows_error(GetLastError());
        return current_status;
    }

#else

    if (::dup2(destination.impl_->fd, STDERR_FILENO) < 0) {
        const int error = errno;
        set_error(map_errno(error), error);
        return current_status;
    }

#endif

    set_error(IOStatus::Success);
    return IOStatus::Success;
}

IOStatus restore_standard_streams() noexcept {
    /*
     * TODO:
     * Save the original descriptors/handles before the first
     * redirection and restore them here.
     *
     * This requires explicit state and lifecycle management.
     */

    set_error(IOStatus::NotSupported);
    return IOStatus::NotSupported;
}

/* ============================================================
 * Public error API
 * ============================================================ */

IOStatus last_error() noexcept {
    return current_status;
}

int last_system_error() noexcept {
    return current_system_error;
}

std::string_view status_message(IOStatus status) noexcept {
    switch (status) {
        case IOStatus::Success:
            return "Success";

        case IOStatus::EndOfFile:
            return "End of file";

        case IOStatus::WouldBlock:
            return "Operation would block";

        case IOStatus::Interrupted:
            return "Operation interrupted";

        case IOStatus::InvalidStream:
            return "Invalid stream";

        case IOStatus::InvalidArgument:
            return "Invalid argument";

        case IOStatus::PermissionDenied:
            return "Permission denied";

        case IOStatus::NotFound:
            return "Resource not found";

        case IOStatus::AlreadyExists:
            return "Resource already exists";

        case IOStatus::NotSupported:
            return "Operation not supported";

        case IOStatus::OutOfMemory:
            return "Out of memory";

        case IOStatus::SystemError:
            return "System error";
    }

    return "Unknown I/O status";
}

} // namespace optic::io
