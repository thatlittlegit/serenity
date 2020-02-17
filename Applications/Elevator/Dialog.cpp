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

#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/TextEditor.h>
#include <LibGUI/Widget.h>
#include <LibGfx/Font.h>

#include "Dialog.h"

namespace Elevator {
bool Dialog::show(const StringView& command, bool needs_password)
{
    return Dialog::construct(command, needs_password)->exec();
}

Dialog::Dialog(const StringView& command, bool needs_password, Core::Object* parent)
    : GUI::Dialog(parent)
{
    auto content = GUI::Widget::construct();
    set_main_widget(content);
    content->set_fill_with_background_color(true);
    content->set_layout(make<GUI::VerticalBoxLayout>());
    content->layout()->set_margins({ 16, 16, 16, 16 });
    content->layout()->set_spacing(8);

    auto label = GUI::Label::construct(content);
    label->set_font(Gfx::Font::default_bold_font());
    label->set_text("A process wants to run the following command as an administrator:");
    label->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
    label->set_text_alignment(Gfx::TextAlignment::TopLeft);
    label->set_preferred_size(0, 11);

    auto command_label = GUI::Label::construct(content);
    command_label->set_font(Gfx::Font::default_fixed_width_font());
    command_label->set_text(command);
    command_label->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
    command_label->set_preferred_size(0, 11);

    if (needs_password) {
        auto description = GUI::Label::construct(content);
        description->set_font(Gfx::Font::default_font());
        description->set_text("You are not an administrative user, so you cannot authorise this.\nPlease ask an administrator to input their information.");
        description->set_text_alignment(Gfx::TextAlignment::TopLeft);
        description->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
        description->set_preferred_size(0, 25);

        auto username_set = GUI::Widget::construct(content.ptr());
        username_set->set_layout(make<GUI::HorizontalBoxLayout>());
        username_set->layout()->set_spacing(8);
        username_set->set_preferred_size(0, 21);
        username_set->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
        auto username_label = GUI::Label::construct(username_set);
        username_label->set_text("Username:");
        auto username_box = GUI::TextEditor::construct(GUI::TextEditor::Type::SingleLine, username_set);

        auto password_set = GUI::Widget::construct(content.ptr());
        password_set->set_layout(make<GUI::HorizontalBoxLayout>());
        password_set->layout()->set_spacing(8);
        password_set->set_preferred_size(0, 21);
        password_set->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
        auto password_label = GUI::Label::construct(password_set);
        password_label->set_text("Password:");
        auto password_box = GUI::TextEditor::construct(GUI::TextEditor::Type::SingleLine, password_set);
    } else {
        auto description = GUI::Label::construct(content);
        description->set_font(Gfx::Font::default_font());
        description->set_text("You are an administrator, so you can authorize this.");
        description->set_text_alignment(Gfx::TextAlignment::TopLeft);
        description->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
        description->set_preferred_size(0, 25);
    }

    auto button_set = GUI::Widget::construct(content.ptr());
    button_set->set_layout(make<GUI::HorizontalBoxLayout>());
    button_set->layout()->set_spacing(8);
    button_set->set_preferred_size(0, 21);
    button_set->set_size_policy(GUI::SizePolicy::Fill, GUI::SizePolicy::Fixed);
    auto authorize_button = GUI::Button::construct(button_set);
    authorize_button->set_text("Authorize");
    authorize_button->set_size_policy(GUI::SizePolicy::Fixed, GUI::SizePolicy::Fixed);
    authorize_button->set_preferred_size(100, 20);
    authorize_button->on_click = [this, needs_password, &command](GUI::Button&) {
        if (needs_password) {
            GUI::MessageBox::show("SerenityOS does not currently support passwords.", "Authentication error", GUI::MessageBox::Type::Error);
            done(false);
        } else {
            done(true);
        }
    };
    auto quit_button = GUI::Button::construct(button_set);
    quit_button->set_text("Deny");
    quit_button->set_size_policy(GUI::SizePolicy::Fixed, GUI::SizePolicy::Fixed);
    quit_button->set_preferred_size(100, 20);
    quit_button->on_click = [this](GUI::Button&) {
        done(false);
    };
    quit_button->set_focus(true);
}
}
