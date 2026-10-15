/* io.hpp */

#pragma once

/*
 * Optic I/O subsystem.
 *
 * Cross-platform stream and file operations.
 * Platform-specific implementation belongs in io.cpp / platform backends.
 */

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace optic::io {

/* ============================================================
 * Types
 * ============================================================ */

/* */
enum class StreamType {
    StandardInput,
    StandardOutput,
    StandardError,
    File,
    Pipe,
    Custom
};

/* */
enum class OpenMode {
    Read,
    Write,
    Append,
    ReadWrite,
    WriteRead,
    AppendRead
};

/* */
enum class SeekOrigin {
    Begin,
    Current,
    End
};

/* */
enum class BufferMode {
    None,
    Line,
    Full
};

/* */
enum class IOStatus {
    Success,
    EndOfFile,
    WouldBlock,
    Interrupted,
    InvalidStream,
    InvalidArgument,
    PermissionDenied,
    NotFound,
    AlreadyExists,
    NotSupported,
    OutOfMemory,
    SystemError
};

/* */
struct IOResult {
    IOStatus status = IOStatus::Success;
    std::size_t bytes_transferred = 0;

    [[nodiscard]]
    bool success() const noexcept {
        return status == IOStatus::Success;
    }

    [[nodiscard]]
    bool eof() const noexcept {
        return status == IOStatus::EndOfFile;
    }
};

/* */
struct StreamInfo {
    StreamType type = StreamType::Custom;
    bool       readable = false;
    bool       writable = false;
    bool       seekable = false;
    bool       terminal = false;
    bool       owned    = false;
};

/* ============================================================
 * Stream
 * ============================================================ */

/*
 * Represents an I/O stream.
 *
 * The native implementation is hidden behind PIMPL so that
 * windows.h and unistd.h are not required by this header.
 *
 * Streams are movable but not copyable.
 */

class Stream {
public:
    Stream() noexcept;

    ~Stream();

    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;

    Stream(Stream&& other) noexcept;
    Stream& operator=(Stream&& other) noexcept;

    /* State */

    [[nodiscard]]
    bool is_open() const noexcept;

    [[nodiscard]]
    bool is_readable() const noexcept;

    [[nodiscard]]
    bool is_writable() const noexcept;

    [[nodiscard]]
    bool is_terminal() const noexcept;

    [[nodiscard]]
    bool is_seekable() const noexcept;

    [[nodiscard]]
    StreamType type() const noexcept;

    /* Stream operations */

    IOResult read(
        void*       buffer,
        std::size_t size
    ) noexcept;

    IOResult write(
        const void* buffer,
        std::size_t size
    ) noexcept;

    IOStatus flush() noexcept;

    IOStatus close() noexcept;

    IOStatus seek(
        std::int64_t offset,
        SeekOrigin   origin
    ) noexcept;

    IOStatus tell(
        std::int64_t* position
    ) noexcept;

    IOStatus get_info(
        StreamInfo* info
    ) const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    explicit Stream(std::unique_ptr<Impl> impl) noexcept;

    friend IOStatus open_file(
        Stream&,
        const std::filesystem::path&,
        OpenMode
    ) noexcept;

    friend Stream& standard_input() noexcept;
    friend Stream& standard_output() noexcept;
    friend Stream& standard_error() noexcept;
};

/* ============================================================
 * Standard streams
 * ============================================================ */

/* */
Stream& standard_input() noexcept;

/* */
Stream& standard_output() noexcept;

/* */
Stream& standard_error() noexcept;

/* ============================================================
 * File operations
 * ============================================================ */

/* */
IOStatus open_file(
    Stream&                      stream,
    const std::filesystem::path& path,
    OpenMode                     mode
) noexcept;

/* */
bool file_exists(
    const std::filesystem::path& path
) noexcept;

/* */
IOStatus remove_file(
    const std::filesystem::path& path
) noexcept;

/* */
IOStatus rename_file(
    const std::filesystem::path& old_path,
    const std::filesystem::path& new_path
) noexcept;

/* ============================================================
 * Raw I/O
 * ============================================================ */

/* */
IOResult read(
    Stream&     stream,
    void*       buffer,
    std::size_t size
) noexcept;

/* */
IOResult write(
    Stream&     stream,
    const void* buffer,
    std::size_t size
) noexcept;

/* */
IOResult read_exact(
    Stream&     stream,
    void*       buffer,
    std::size_t size
) noexcept;

/* */
IOResult write_all(
    Stream&     stream,
    const void* buffer,
    std::size_t size
) noexcept;

/* ============================================================
 * Text I/O
 * ============================================================ */

/* */
IOStatus read_line(
    Stream&      stream,
    std::string& output
) noexcept;

/* */
IOStatus read_text(
    Stream&      stream,
    std::string& output,
    std::size_t  max_size
) noexcept;

/* */
IOResult write_text(
    Stream&          stream,
    std::string_view text
) noexcept;

/* */
IOResult write_line(
    Stream&          stream,
    std::string_view text
) noexcept;

/* ============================================================
 * Buffering
 * ============================================================ */

/* */
IOStatus set_buffer_mode(
    Stream&    stream,
    BufferMode mode
) noexcept;

/* */
IOStatus sync_stream(
    Stream& stream
) noexcept;

/* ============================================================
 * Stream redirection
 * ============================================================ */

/* */
IOStatus redirect_standard_input(
    Stream& source
) noexcept;

/* */
IOStatus redirect_standard_output(
    Stream& destination
) noexcept;

/* */
IOStatus redirect_standard_error(
    Stream& destination
) noexcept;

/* */
IOStatus restore_standard_streams() noexcept;

/* ============================================================
 * Error handling
 * ============================================================ */

/* */
IOStatus last_error() noexcept;

/* */
int last_system_error() noexcept;

/* */
std::string_view status_message(
    IOStatus status
) noexcept;

} // namespace optic::io