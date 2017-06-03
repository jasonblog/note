/*
 * Copyright (C) 2016 E.S.R.Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDROID_FILE_H_
#define MINDROID_FILE_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"

namespace mindroid {

class FilenameFilter;

class File :
        public Object {
public:
    /**
     * The system-dependent string used to separate components in search paths (":").
     * See {@link #pathSeparatorChar}.
     */
    static const sp<String> pathSeparator;

    /**
     * The system-dependent character used to separate components in search paths (':').
     * This is used to split such things as the PATH environment variable and classpath
     * system properties into lists of directories to be searched.
     *
     * <p>This field is initialized from the system property "path.separator".
     * Later changes to that property will have no effect on this field or this class.
     */
    static const char pathSeparatorChar = ':';

    /**
     * The system-dependent string used to separate components in filenames ('/').
     * See {@link #separatorChar}.
     */
    static const sp<String> separator;

    /**
     * The system-dependent character used to separate components in filenames ('/').
     * Use of this (rather than hard-coding '/') helps portability to other operating systems.
     *
     * <p>This field is initialized from the system property "file.separator".
     * Later changes to that property will have no effect on this field or this class.
     */
    static const char separatorChar = '/';

    /**
     * Constructs a new file using the specified directory and name.
     *
     * @param dir
     *            the directory where the file is stored.
     * @param name
     *            the file's name.
     */
    File(const sp<File>& dir, const char* name);
    File(const sp<File>& dir, const sp<String>& name);

    /**
     * Constructs a new file using the specified path.
     *
     * @param path
     *            the path to be used for the file.
     */
    File(const char* path);
    File(const sp<String>& path);

    /**
     * Constructs a new File using the specified directory path and file name,
     * placing a path separator between the two.
     *
     * @param dirPath
     *            the path to the directory where the file is stored.
     * @param name
     *            the file's name.
     */
    File(const char* dirPath, const char* name);
    File(const sp<String>& dirPath, const sp<String>& name);

    virtual ~File() { }

    /**
     * Tests whether or not this process is allowed to execute this file.
     * Note that this is a best-effort result; the only way to be certain is
     * to actually attempt the operation.
     *
     * @return {@code true} if this file can be executed, {@code false} otherwise.
     */
    bool canExecute() const;

    /**
     * Indicates whether the current context is allowed to read from this file.
     *
     * @return {@code true} if this file can be read, {@code false} otherwise.
     */
    bool canRead() const;

    /**
     * Indicates whether the current context is allowed to write to this file.
     *
     * @return {@code true} if this file can be written, {@code false}
     *         otherwise.
     */
    bool canWrite() const;

    /**
     * Removes this file. Directories must be empty before they will be deleted.
     *
     * @return {@code true} if this file was deleted, {@code false} otherwise.
     */
    bool remove();

    /**
     * Returns a boolean indicating whether this file can be found on the
     * underlying file system.
     *
     * @return {@code true} if this file exists, {@code false} otherwise.
     */
    bool exists() const;

    /**
     * Returns the absolute path of this file. An absolute path is a path that starts at a root
     * of the file system.
     *
     * <p>A common use for absolute paths is when passing paths to a {@code Process} as
     * command-line arguments, to remove the requirement implied by relative paths, that the
     * child must have the same working directory as its parent.
     */
    sp<String> getAbsolutePath();

    /**
     * Returns the name of the file or directory represented by this file.
     *
     * @return this file's name or an empty string if there is no name part in
     *         the file's path.
     */
    sp<String> getName();

    /**
     * Returns the name of the file or directory represented by this file.
     *
     * @return this file's name or an empty string if there is no name part in
     *         the file's path.
     */
    sp<String> getParent();

    /**
     * Returns a new file made from the pathname of the parent of this file.
     * This is the path up to but not including the last name. {@code null} is
     * returned when there is no parent.
     *
     * @return a new file representing this file's parent or {@code null}.
     */
    sp<File> getParentFile();

    /**
     * Returns the path of this file.
     */
    sp<String> getPath();

    /**
     * Indicates if this file's pathname is absolute. Whether a pathname is
     * absolute is platform specific. On Mindroid, absolute paths start with
     * the character '/'.
     *
     * @return {@code true} if this file's pathname is absolute, {@code false}
     *         otherwise.
     * @see #getPath
     */
    bool isAbsolute() const;

    /**
     * Indicates if this file represents a <em>directory</em> on the
     * underlying file system.
     *
     * @return {@code true} if this file is a directory, {@code false}
     *         otherwise.
     */
    bool isDirectory() const;

    /**
     * Indicates if this file represents a <em>file</em> on the underlying
     * file system.
     *
     * @return {@code true} if this file is a file, {@code false} otherwise.
     */
    bool isFile() const;

    /**
     * Returns the length of this file in bytes.
     * Returns 0 if the file does not exist.
     * The result for a directory is not defined.
     *
     * @return the number of bytes in this file.
     */
    size_t length() const;

    /**
     * Returns an array of files contained in the directory represented by this
     * file. The result is {@code null} if this file is not a directory. The
     * paths of the files in the array are absolute if the path of this file is
     * absolute, they are relative otherwise.
     *
     * @return an array of files or {@code null}.
     */
    sp<ArrayList<sp<File>>> listFiles();

    /**
     * Gets a list of the files in the directory represented by this file. This
     * list is then filtered through a FilenameFilter and files with matching
     * names are returned as an array of files. Returns {@code null} if this
     * file is not a directory. If {@code filter} is {@code null} then all
     * filenames match.
     * <p>
     * The entries {@code .} and {@code ..} representing the current and parent
     * directories are not returned as part of the list.
     *
     * @param filter
     *            the filter to match names against, may be {@code null}.
     * @return an array of files or {@code null}.
     */
    sp<ArrayList<sp<File>>> listFiles(const sp<FilenameFilter>& filter);

    /**
     * Creates the directory named by this file, assuming its parents exist.
     * Use {@link #mkdirs} if you also want to create missing parents.
     *
     * @return {@code true} if the directory was created,
     *         {@code false} on failure or if the directory already existed.
     */
    bool mkdir();

    /**
     * Creates a new, empty file on the file system according to the path
     * information stored in this file. This method returns true if it creates
     * a file, false if the file already existed. Note that it returns false
     * even if the file is not a file (because it's a directory, say).
     *
     * <p>This method is not generally useful. For creating temporary files,
     * use {@link #createTempFile} instead. For reading/writing files, use {@link FileInputStream},
     * {@link FileOutputStream}, or {@link RandomAccessFile}, all of which can create files.
     *
     * <p>Note that this method does <i>not</i> throw {@code IOException} if the file
     * already exists, even if it's not a regular file. Callers should always check the
     * return value, and may additionally want to call {@link #isFile}.
     *
     * @return true if the file has been created, false if it
     *         already exists.
     * @throws IOException if it's not possible to create the file.
     */
    bool createNewFile();

    /**
     * Renames this file to {@code newPath}. This operation is supported for both
     * files and directories.
     *
     * <p>Many failures are possible. Some of the more likely failures include:
     * <ul>
     * <li>Write permission is required on the directories containing both the source and
     * destination paths.
     * <li>Search permission is required for all parents of both paths.
     * </ul>
     *
     * @param newPath the new path.
     * @return true on success.
     */
    bool renameTo(const sp<File>& newPath);

    /**
     * Returns a string containing a concise, human-readable description of this
     * file.
     *
     * @return a printable representation of this file.
     */
    sp<String> toString() {
        return mPath;
    }

private:
    static sp<String> join(sp<String>& prefix, sp<String>& suffix);

    sp<String> mPath;
    sp<String> mParent;
    sp<String> mName;
};

} /* namespace mindroid */

#endif /* MINDROID_FILE_H_ */
