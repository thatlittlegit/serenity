/*
 * Copyright (c) 2020, the SerenityOS developers.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/ByteBuffer.h>
#include <AK/NonnullRefPtr.h>
#include <AK/Optional.h>
#include <AK/String.h>
#include <LibCore/File.h>
#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/Desktop.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/TextEditor.h>
#include <LibGUI/Window.h>
#include <LibGfx/Font.h>
#include <LibGfx/TextAlignment.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "Dialog.h"

Optional<NonnullRefPtr<Core::File>> open_file(String path)
{
    auto file = Core::File::construct(path);
    if (!file->open(Core::IODevice::ReadOnly))
        return Optional<NonnullRefPtr<Core::File>>();

    return file;
}

Optional<String> get_line_from_file(NonnullRefPtr<Core::File> file)
{
    auto line = file->read_line(512);

    if (line.is_null()) {
        if (file->error()) {
            file->close();
            return Optional<String>();
        }

        return String::empty();
    }

    return String((char*)line.data(), line.size());
}

Optional<String> get_username_for_uid(uid_t uid)
{
    auto file = open_file("/etc/passwd").value();
    auto string_uid = String::number(uid);

    while (true) {
        auto line = get_line_from_file(file).value();
        ASSERT(!line.is_empty());

        auto parts = line.split(':');
        if (parts[2] == string_uid) {
            file->close();
            return parts[0];
        }
    }
}

bool needs_password(uid_t uid)
{
    auto file = open_file("/etc/group").value();
    auto username = get_username_for_uid(uid).value();

    while (true) {
        auto line = get_line_from_file(file).value();
        ASSERT(!line.is_empty());

        if (line.starts_with("wheel:"))
            return !line.contains(username);
    }
}

static int ask_for_consent(int argc, char** argv, char* command)
{
    if (pledge("stdio shared_buffer rpath unix cpath fattr id exec", nullptr) < 0) {
        perror("pledge");
        return 2;
    }

    GUI::Application app(argc, argv);

    if (pledge("stdio shared_buffer rpath id exec", nullptr) < 0) {
        perror("pledge");
        return 2;
    }

    return Elevator::Dialog::show(command, needs_password(getuid()));
}

int main(int argc, char** argv)
{
    auto command = argv[1];

    auto consent = ask_for_consent(argc, argv, command);
    if (consent >= 2)
        return 1;
    else if (consent == 0)
        return 0;

    if (pledge("stdio id exec shared_buffer", nullptr) < 0) {
        perror("pledge");
        return 2;
    }

    setuid(0);
    if (execvp(command, argv + 1) < 0) {
        perror("exec");
        return 1;
    }

    ASSERT_NOT_REACHED();
}
