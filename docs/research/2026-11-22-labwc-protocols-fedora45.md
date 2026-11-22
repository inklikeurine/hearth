# labwc on Fedora 45: which protocols it offers

*Checked by Claude on 2026-11-22 in the development box (see [dev environment](../dev-environment.md)). Tags: [C] = seen on this run, [S] = secondary, [M] = from memory.*

**Answer:** Fedora 45's labwc 0.20.2 offers every protocol the first spike needs: overlays (layer-shell), the on-screen keyboard trio (input-method v2, text-input v3, virtual-keyboard v1), switching apps (foreign-toplevel), and display settings (output management). [C]

## Method

labwc 0.20.2-2.fc45 was started headless (no screen, software rendering) inside the box, and `wayland-info` listed what it offers:

```sh
WLR_BACKENDS=headless WLR_RENDERER=pixman WLR_LIBINPUT_NO_DEVICES=1 labwc -S wayland-info
```

## What Hearth needs, and what labwc offered

| Need | Protocol (version offered) | Result |
|---|---|---|
| Menu, volume and status overlays drawn over an app | `zwlr_layer_shell_v1` (v4) | Offered [C] |
| Hearth's keyboard learns a text box is focused, sends text back | `zwp_input_method_manager_v2` (v1) | Offered [C] |
| The app's side of the same (Firefox, SDL3, GTK4 use it) | `zwp_text_input_manager_v3` (v1) | Offered [C] |
| Keys with no text: Enter, arrows, Backspace | `zwp_virtual_keyboard_manager_v1` (v1) | Offered [C] |
| Home screen lists apps and brings one to the front | `zwlr_foreign_toplevel_manager_v1` (v3), `ext_foreign_toplevel_list_v1` (v1) | Offered [C] |
| Settings → Display: resolution, refresh rate | `zwlr_output_manager_v1` (v4) | Offered [C] |
| Screen off / on | `zwlr_output_power_manager_v1` (v1) | Offered [C] |
| Screensaver after idle, and "don't sleep while playing" | `ext_idle_notifier_v1` (v2), `zwp_idle_inhibit_manager_v1` (v1) | Offered [C] |
| Limit what sandboxed apps (Flatpaks) may use | `wp_security_context_manager_v1` (v1) | Offered [C] |
| HDR colour management | `wp_color_management_v1` | Not offered on this run. Expected: labwc enables HDR10 only when it's switched on in its config and the Vulkan renderer is used ([compositor accessibility and HDR](E3-compositor-a11y-hdr.md)); this run had HDR off and software rendering |

All 53 interfaces offered: `ext_data_control_manager_v1`, `ext_foreign_toplevel_image_capture_source_manager_v1`, `ext_foreign_toplevel_list_v1`, `ext_idle_notifier_v1`, `ext_image_copy_capture_manager_v1`, `ext_output_image_capture_source_manager_v1`, `ext_session_lock_manager_v1`, `ext_workspace_manager_v1`, `org_kde_kwin_server_decoration_manager`, `wl_compositor`, `wl_data_device_manager`, `wl_fixes`, `wl_output`, `wl_seat`, `wl_shm`, `wl_subcompositor`, `wp_alpha_modifier_v1`, `wp_color_representation_manager_v1`, `wp_cursor_shape_manager_v1`, `wp_fractional_scale_manager_v1`, `wp_presentation`, `wp_security_context_manager_v1`, `wp_single_pixel_buffer_manager_v1`, `wp_tearing_control_manager_v1`, `wp_viewporter`, `xdg_activation_v1`, `xdg_toplevel_icon_manager_v1`, `xdg_wm_base`, `xdg_wm_dialog_v1`, `zwlr_data_control_manager_v1`, `zwlr_export_dmabuf_manager_v1`, `zwlr_foreign_toplevel_manager_v1`, `zwlr_gamma_control_manager_v1`, `zwlr_layer_shell_v1`, `zwlr_output_manager_v1`, `zwlr_output_power_manager_v1`, `zwlr_screencopy_manager_v1`, `zwlr_virtual_pointer_manager_v1`, `zwp_idle_inhibit_manager_v1`, `zwp_input_method_manager_v2`, `zwp_pointer_constraints_v1`, `zwp_pointer_gestures_v1`, `zwp_primary_selection_device_manager_v1`, `zwp_relative_pointer_manager_v1`, `zwp_tablet_manager_v2`, `zwp_text_input_manager_v3`, `zwp_virtual_keyboard_manager_v1`, `zxdg_decoration_manager_v1`, `zxdg_exporter_v1`, `zxdg_exporter_v2`, `zxdg_importer_v1`, `zxdg_importer_v2`, `zxdg_output_manager_v1`. [C]

## Not confirmed

- That each protocol works end to end (an overlay over full-screen Firefox, typing into Firefox): that's the spike.
- TV colour range: the output-management protocol has no colour-range setting, so labwc can't be asked for one this way [M]. Not checked in the protocol file.
- labwc logged that Xwayland (the layer that runs old X11 apps) isn't installed and carried on without it. The box doesn't install it; Firefox runs natively on Wayland [C].
- Everything on real hardware (direct to the screen, not headless).
