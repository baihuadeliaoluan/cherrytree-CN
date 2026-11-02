/*
 * ct_pref_dlg_typography.cc
 *
 * Chinese typography preferences (added by cherrytree-CN fork):
 * font, editor font size, line height, paragraph spacing,
 * editor width, horizontal/vertical page margins.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ct_pref_dlg.h"
#include "ct_main_win.h"
#include "ct_misc_utils.h"

Gtk::Widget* CtPrefDlg::build_tab_typography()
{
#if GTKMM_MAJOR_VERSION >= 4
    auto box = Gtk::manage(new Gtk::Box{Gtk::Orientation::VERTICAL, 6/*spacing*/});
    box->append(*Gtk::manage(new Gtk::Label{_("Chinese Typography preferences are temporarily limited in GTK4 build.")}));
    return box;
#else
    auto pGrid = Gtk::manage(new Gtk::Grid{});
    pGrid->set_margin_start(10);
    pGrid->set_margin_end(10);
    pGrid->set_margin_top(10);
    pGrid->set_margin_bottom(10);
    pGrid->set_row_spacing(8);
    pGrid->set_column_spacing(12);
    int row = 0;

    // apply-on-change helper: typography is applied live to every open window
    auto f_apply_typography = [this]() {
        apply_for_each_window([](CtMainWin* win) { win->text_typography_apply(); });
    };

    // Font
    {
        auto label_font = Gtk::manage(new Gtk::Label{_("Font")});
        gtk_label_set_xalign(label_font->gobj(), 0.0);
        auto font_button = Gtk::manage(new Gtk::FontButton{_pConfig->rtFont});
        font_button->set_show_size(true);
        font_button->signal_font_set().connect([this, font_button, f_apply_typography](){
            const int currSize = CtFontUtil::get_font_size(_pConfig->rtFont);
            const int newSize = CtFontUtil::get_font_size(font_button->get_font_name());
            const int useSize = newSize > 0 ? newSize : currSize;
            _pConfig->rtFont = CtFontUtil::get_font_str(CtFontUtil::get_font_family(font_button->get_font_name()), useSize);
            f_apply_typography();
        });
        pGrid->attach(*label_font, 0, row, 1, 1);
        pGrid->attach(*font_button, 1, row, 2, 1);
        ++row;
    }

    // Editor Font Size (splits size out of the rich text font string)
    {
        auto label_size = Gtk::manage(new Gtk::Label{_("Editor Font Size")});
        gtk_label_set_xalign(label_size->gobj(), 0.0);
        Glib::RefPtr<Gtk::Adjustment> adj_size = Gtk::Adjustment::create(CtFontUtil::get_font_size(_pConfig->rtFont), 9, 36, 1);
        auto scale_size = Gtk::manage(new Gtk::HScale{adj_size});
        scale_size->set_size_request(200, -1);
        scale_size->set_draw_value(false);
        auto spin_size = Gtk::manage(new Gtk::SpinButton{adj_size});
        spin_size->set_digits(0);
        adj_size->signal_value_changed().connect([this, adj_size, f_apply_typography](){
            const int newSize = static_cast<int>(adj_size->get_value());
            _pConfig->rtFont = CtFontUtil::get_font_str(CtFontUtil::get_font_family(_pConfig->rtFont), newSize);
            f_apply_typography();
        });
        pGrid->attach(*label_size, 0, row, 1, 1);
        pGrid->attach(*scale_size, 1, row, 1, 1);
        pGrid->attach(*spin_size, 2, row, 1, 1);
        ++row;
    }

    // Line Height (ratio, applied as extra pixels between wrapped lines)
    {
        auto label_line_spacing = Gtk::manage(new Gtk::Label{_("Line Height")});
        gtk_label_set_xalign(label_line_spacing->gobj(), 0.0);
        Glib::RefPtr<Gtk::Adjustment> adj_line_spacing = Gtk::Adjustment::create(_pConfig->lineSpacing, 1.0, 2.5, 0.05);
        auto scale_line_spacing = Gtk::manage(new Gtk::HScale{adj_line_spacing});
        scale_line_spacing->set_size_request(200, -1);
        scale_line_spacing->set_draw_value(false);
        auto spin_line_spacing = Gtk::manage(new Gtk::SpinButton{adj_line_spacing});
        spin_line_spacing->set_digits(2);
        adj_line_spacing->signal_value_changed().connect([this, adj_line_spacing, f_apply_typography](){
            _pConfig->lineSpacing = adj_line_spacing->get_value();
            f_apply_typography();
        });
        pGrid->attach(*label_line_spacing, 0, row, 1, 1);
        pGrid->attach(*scale_line_spacing, 1, row, 1, 1);
        pGrid->attach(*spin_line_spacing, 2, row, 1, 1);
        ++row;
    }

    // Paragraph Spacing (vertical space around lines)
    {
        auto label_paragraph = Gtk::manage(new Gtk::Label{_("Paragraph Spacing")});
        gtk_label_set_xalign(label_paragraph->gobj(), 0.0);
        Glib::RefPtr<Gtk::Adjustment> adj_paragraph = Gtk::Adjustment::create(_pConfig->spaceAroundLines, 0, 30, 1);
        auto scale_paragraph = Gtk::manage(new Gtk::HScale{adj_paragraph});
        scale_paragraph->set_size_request(200, -1);
        scale_paragraph->set_draw_value(false);
        auto spin_paragraph = Gtk::manage(new Gtk::SpinButton{adj_paragraph});
        spin_paragraph->set_digits(0);
        adj_paragraph->signal_value_changed().connect([this, adj_paragraph, f_apply_typography](){
            _pConfig->spaceAroundLines = static_cast<int>(adj_paragraph->get_value());
            f_apply_typography();
        });
        pGrid->attach(*label_paragraph, 0, row, 1, 1);
        pGrid->attach(*scale_paragraph, 1, row, 1, 1);
        pGrid->attach(*spin_paragraph, 2, row, 1, 1);
        ++row;
    }

    // Editor Width (0 = full width, >0 = centered content column)
    {
        auto label_width = Gtk::manage(new Gtk::Label{_("Editor Width (0 = Full Width)")});
        gtk_label_set_xalign(label_width->gobj(), 0.0);
        Glib::RefPtr<Gtk::Adjustment> adj_width = Gtk::Adjustment::create(_pConfig->editorWidth, 0, 1400, 10);
        auto scale_width = Gtk::manage(new Gtk::HScale{adj_width});
        scale_width->set_size_request(200, -1);
        scale_width->set_draw_value(false);
        auto spin_width = Gtk::manage(new Gtk::SpinButton{adj_width});
        spin_width->set_digits(0);
        adj_width->signal_value_changed().connect([this, adj_width, f_apply_typography](){
            _pConfig->editorWidth = static_cast<int>(adj_width->get_value());
            f_apply_typography();
        });
        pGrid->attach(*label_width, 0, row, 1, 1);
        pGrid->attach(*scale_width, 1, row, 1, 1);
        pGrid->attach(*spin_width, 2, row, 1, 1);
        ++row;
    }

    // Horizontal Page Margin (left = right)
    {
        auto label_hmargin = Gtk::manage(new Gtk::Label{_("Horizontal Page Margin")});
        gtk_label_set_xalign(label_hmargin->gobj(), 0.0);
        Glib::RefPtr<Gtk::Adjustment> adj_hmargin = Gtk::Adjustment::create(_pConfig->textMarginLeft, 0, 200, 1);
        auto scale_hmargin = Gtk::manage(new Gtk::HScale{adj_hmargin});
        scale_hmargin->set_size_request(200, -1);
        scale_hmargin->set_draw_value(false);
        auto spin_hmargin = Gtk::manage(new Gtk::SpinButton{adj_hmargin});
        spin_hmargin->set_digits(0);
        adj_hmargin->signal_value_changed().connect([this, adj_hmargin, f_apply_typography](){
            const int val = static_cast<int>(adj_hmargin->get_value());
            _pConfig->textMarginLeft = val;
            _pConfig->textMarginRight = val;
            f_apply_typography();
        });
        pGrid->attach(*label_hmargin, 0, row, 1, 1);
        pGrid->attach(*scale_hmargin, 1, row, 1, 1);
        pGrid->attach(*spin_hmargin, 2, row, 1, 1);
        ++row;
    }

    // Vertical Page Margin (top = bottom)
    {
        auto label_vmargin = Gtk::manage(new Gtk::Label{_("Vertical Page Margin")});
        gtk_label_set_xalign(label_vmargin->gobj(), 0.0);
        Glib::RefPtr<Gtk::Adjustment> adj_vmargin = Gtk::Adjustment::create(_pConfig->textMarginTop, 0, 100, 1);
        auto scale_vmargin = Gtk::manage(new Gtk::HScale{adj_vmargin});
        scale_vmargin->set_size_request(200, -1);
        scale_vmargin->set_draw_value(false);
        auto spin_vmargin = Gtk::manage(new Gtk::SpinButton{adj_vmargin});
        spin_vmargin->set_digits(0);
        adj_vmargin->signal_value_changed().connect([this, adj_vmargin, f_apply_typography](){
            const int val = static_cast<int>(adj_vmargin->get_value());
            _pConfig->textMarginTop = val;
            _pConfig->textMarginBottom = val;
            f_apply_typography();
        });
        pGrid->attach(*label_vmargin, 0, row, 1, 1);
        pGrid->attach(*scale_vmargin, 1, row, 1, 1);
        pGrid->attach(*spin_vmargin, 2, row, 1, 1);
        ++row;
    }

    // Justify Fill Whole Document hint
    {
        auto label_justify = Gtk::manage(new Gtk::Label{_("Tip: Format menu > Justify Fill All Paragraphs aligns the whole document.")});
        gtk_label_set_xalign(label_justify->gobj(), 0.0);
        label_justify->get_style_context()->add_class("dim-label");
        pGrid->attach(*label_justify, 0, row, 3, 1);
        ++row;
    }

    auto pScrolled = Gtk::manage(new Gtk::ScrolledWindow{});
    pScrolled->add(*pGrid);
    pScrolled->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    return pScrolled;
#endif
}
