#include <cstring>
#include "signature.hpp"
#include "hac/codefinder.h"
#include "../chimera.hpp"
#include "hook.hpp"

namespace Chimera {
    const char *Signature::name() const noexcept {
        return this->p_name.data();
    }
    const char *Signature::feature() const noexcept {
        return this->p_feature.data();
    }
    const std::byte *Signature::original_data() const noexcept {
        return this->p_original_data.data();
    }
    std::size_t Signature::original_data_size() const noexcept {
        return this->p_original_data.size();
    }
    std::byte *Signature::data() const noexcept {
        return this->p_data;
    }

    void Signature::rollback() const noexcept {
        overwrite(this->data(), this->original_data(), this->original_data_size());
    }

    Signature::Signature(const char *name, const char *feature, const SigByte *signature, std::size_t length) : p_name(name), p_feature(feature) {
        this->p_data = reinterpret_cast<std::byte *>(FindCode(GetModuleHandle(nullptr), signature, length));
        if(this->p_data) {
            this->p_original_data.insert(this->p_original_data.begin(), this->p_data, this->p_data + length);
        }
    }

    #define FIND(name, feature, ...) {\
        const SigByte sig_data[] = __VA_ARGS__;\
        signatures.emplace_back(name, feature, sig_data, sizeof(sig_data) / sizeof(*sig_data));\
    }

    std::vector<Signature> find_all_signatures() {
        std::vector<Signature> signatures;

        // Core
        FIND("path_sig", "core", { 0x68, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x54, 0x68, -1, -1, -1, -1, 0x52, 0x8D, 0xA9 });
        FIND("on_tick_sig", "core", { 0xE8, -1, -1, -1, -1, 0xA1, -1, -1, -1, -1, 0x8B, 0x50, 0x14, 0x8B, 0x48, 0x0C });
        FIND("get_crc_sig", "core", { 0xC1, 0xE0, 0x04, 0x8B, 0x4C, 0x10, 0x0C, 0x51, 0x89, 0x8C, 0x24 });
        FIND("loading_map_sig", "core", { 0xB8, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x32, 0xC9, 0x83, 0xF8, 0x13 });
        FIND("console_call_sig", "core", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x88, 0x1D, -1, -1, -1, -1, 0x66, 0x89, 0x1D });
        FIND("console_block_error_sig", "core", { 0x68, -1, -1, -1, -1, 0x53, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x5E, 0x8A, 0xC3, 0x5B, 0x81 });
        FIND("devmode_sig", "core", { 0x74, 0x18, 0x85, 0xC0, 0x7E, 0x39, 0x83, 0xF8, 0x06, 0x7E, 0x6E });
        FIND("server_type_sig", "core", { 0x0F, 0xBF, 0x2D, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x39, 0x1D, -1, -1, -1, -1, 0x75, 0x05 });
        FIND("current_gametype_sig", "core", { 0x83, 0x3D, -1, -1, -1, -1, 0x04, 0x8B, 0x4F, 0x6C, 0x89, 0x4C, 0x24, 0x34, 0x75 });
        FIND("player_table_sig", "core", { 0xA1, -1, -1, -1, -1, 0x89, 0x44, 0x24, 0x48, 0x35 });
        FIND("console_out_sig", "core", { 0x83, 0xEC, 0x10, 0x57, 0x8B, 0xF8, 0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x80, 0x3F });
        FIND("map_index_sig", "core", { 0x8B, 0x15, -1, -1, -1, -1, 0x5F, 0xC1, 0xE1, 0x04, 0x5E });
        FIND("load_multiplayer_maps_sig", "core", {0x81, 0xEC, 0x40, 0x01, 0x00, 0x00, 0x56, 0xBE, -1, -1, -1, -1, 0x8D, 0x64, 0x24, 0x00, 0x8B, 0x46, 0xFC, 0x50});
        FIND("free_map_index_sig", "core", { 0xA1, -1, -1, -1, -1, 0x53, 0x8B, 0x1D, -1, -1, -1, -1, 0x56, 0x33, 0xF6, 0x85, 0xC0 });
        FIND("execute_script_sig", "core", { 0x81, 0xEC, 0x0C, 0x08, 0x00, 0x00, 0x53, 0x55, 0x8B, 0xAC, 0x24, 0x18, 0x08, 0x00, 0x00, 0x68, 0x00, 0x04, 0x00, 0x00 });
        FIND("object_table_sig", "core", { 0x8B, 0x0D, -1, -1, -1, -1, 0x8B, 0x51, 0x34, 0x25, 0xFF, 0xFF, 0x00, 0x00, 0x8D });
        FIND("tick_rate_sig", "core", { 0xD8, 0x0D, -1, -1, -1, -1, 0x83, 0xEC, 0x08, 0xD9, 0x5C, 0x24, 0x08, 0xD9, 0x44, 0x24, 0x14, 0xD8, 0x41, 0x1C });
        FIND("game_speed_sig", "core", { 0xA1, -1, -1, -1, -1, 0x66, 0x89, 0x58, 0x10, 0x66, 0x8B, 0x0D, -1, -1, -1, -1, 0x66, 0x83, 0xF9, 0x01, 0x74, 0x1E, 0x66, 0x83, 0xF9, 0x02 });
        FIND("effect_table_sig", "core", { 0xA1, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x53, 0x8B, 0x5C, 0x24, 0x24, 0x81, 0xE3, 0xFF, 0xFF, 0x00, 0x00 });
        FIND("player_id_sig", "core", { 0x8B, 0x35, -1, -1, -1, -1, 0x8B, 0x7C, 0x24, 0x20, 0x83, 0x7C, 0xBE, 0x04, 0xFF, 0x74 });
        FIND("map_header_sig", "core", { 0x81, 0x3D, -1, -1, -1, -1, 0x64, 0x61, 0x65, 0x68, 0x8B, 0x3D, -1, -1, -1, -1, 0x0F, 0x85 });
        FIND("ce_check_load_sig", "core", {0x81, 0x3D, -1, -1, -1, -1, 0x61, 0x02, 0x00, 0x00, 0x0F, 0x85, -1, 0x04, 0x00, 0x00});
        FIND("ce_check_startup_sig", "core", {0x81, 0x3D, -1, -1, -1, -1, 0x61, 0x02, 0x00, 0x00, 0x0F, 0x85, -1, 0x01, 0x00, 0x00});
        FIND("ce_check_preload_map_list_sig", "core", {0x81, 0x7E, 0x04, 0x61, 0x02, 0x00, 0x00, 0x75, 0x02});
        FIND("ce_check_loading_sig", "core", { 0x81, 0x7F, 0x04, 0x61, 0x02, 0x00, 0x00, 0x74, 0x7A, 0x33, 0xC0 });
        FIND("after_memory_allocated_sig", "core", { 0xFF, 0x15, -1, -1, -1, -1, 0x5F, 0x5E, 0x8B, 0xE5, 0x5D, 0xC3 });
        FIND("read_file_ex_map_sig", "core", { 0xFF, 0x54, 0x24, 0x1C, 0x85, 0xC0, 0x75, 0x29 });
        FIND("loaded_header_sig", "core", { 0x0F, 0xBF, 0x05, -1, -1, -1, -1, 0x69, 0xC0, 0x0C, 0x08, 0x00, 0x00, 0x8B, 0x88, -1, -1, -1, -1, 0x8A, 0x46, 0x20, 0x84, 0xC0, 0x74, 0x24 });
        FIND("map_path_3_sig", "core", { 0x68, -1, -1, -1, -1, 0x52, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x8D, 0x44, 0x24, 0x10, 0x53, 0x50 });

        FIND("server_drm_1_sig", "core_drm", { 0x88, 0x5C, 0x24, 0x2A, 0xE8, -1, -1, -1, -1, 0x50, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x0C });
        FIND("server_drm_2_sig", "core_drm", {0x8B, 0x7F, 0x08, 0x3B, 0xFD, 0x74, 0x5F, 0x8D, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x07, 0x83, 0xC0, 0x04, 0x8D, 0x74, 0x24, 0x34, 0x8D, 0xA4, 0x24, 0x00, 0x00, 0x00, 0x00});

        FIND("leak_file_descriptors_1_sig", "core_file_descriptors", { 0x05, 0x00, 0x00, 0x00, 0x08, 0x3B, 0x44, 0x24, 0x18, 0x7E, 0x64 });
        FIND("leak_file_descriptors_2_sig", "core_file_descriptors", { 0x81, 0xC1, 0x00, 0x00, 0x00, 0x08, 0x66, 0x83, 0xFB, 0x01, 0x7F, 0x07 });
        FIND("leak_file_descriptors_3_sig", "core_file_descriptors", { 0x05, 0x00, 0x00, 0x00, 0x08, 0x3B, 0xC8, 0x7C, 0x12, 0x8D, 0x47, 0x04 });

        // Server
        FIND("map_server_path_1_sig", "server", {0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x56, 0xE8, -1, -1, -1, -1, 0x33, 0xC0});
        FIND("map_server_path_2_sig", "server", {0x68, -1, -1, -1, -1, 0xEB, 0x05, 0x68, -1, -1, -1, -1, 0x68, 0xFF, 0x00, 0x00, 0x00});

        // Client
        FIND("map_client_path_1_sig", "client", {0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x8D, 0x4C, 0x24, 0x18, 0x68, -1, -1, -1, -1, 0x51});
        FIND("map_client_path_2_sig", "client", {0x68, -1, -1, -1, -1, 0xB9, 0x00, 0x02, 0x00, 0x00, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1});

        FIND("load_ui_map_sig", "client", { 0x81, 0xEC, 0x14, 0x01, 0x00, 0x00, 0x53, 0x56, 0x57, 0xBB, 0x01, 0x00, 0x00, 0x00 });
        FIND("decal_table_sig", "client", { 0xA1, -1, -1, -1, -1, 0x8A, 0x48, 0x24, 0x83, 0xEC, 0x10, 0x84, 0xC9, 0x74, 0x48, 0x89, 0x04, 0x24, 0x57, 0x35, 0x72, 0x65, 0x74, 0x69 });
        FIND("on_frame_sig", "client", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x08, 0x89, 0x3D });
        FIND("on_camera_sig", "client", { 0xE8, -1, -1, -1, -1, 0x8B, 0x45, 0xEC, 0x8B, 0x4D, 0xF0, 0x40, 0x81, 0xC6 });
        FIND("camera_coord_sig", "client", { 0xD9, 0x1D, -1, -1, -1, -1, 0xD9, 0x05, -1, -1, -1, -1, 0xD8, 0x44, 0x24, 0x08, 0xD9, 0x1D, -1, -1, -1, -1, 0xD9, 0x05, -1, -1, -1, -1, 0xD8, 0x44, 0x24, 0x0C });
        FIND("camera_type_sig", "client", { 0x81, 0xC1, -1, -1, -1, -1, 0x8B, 0x41, 0x08, 0x3D, -1, -1, -1, -1, 0x75, 0x1D, 0xD9, 0x05 });
        FIND("hud_message_sig", "client", { 0x66, 0x3D, 0xFF, 0xFF, 0x74, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x56, 0x57, 0x0F, 0xBF, 0xF8, 0x69, 0xFF, 0x60, 0x04, 0x00, 0x00, 0x03, 0xFA, 0x6A, 0x00 });
        FIND("alpha_blend_camo_sig", "client", { 0xC7, 0x44, 0x24, -1, 0x66, 0x66, 0x66, 0x3F, 0xA1 });
        FIND("dart_1_sig", "client", { 0xC6, 0x05, -1, -1, -1, -1, 0x01, 0xC6, 0x05, -1, -1, -1, -1, 0x01, 0x39, 0x1D, -1, -1, -1, -1, 0x74, 0x07, 0xC6, 0x05, -1, -1, -1, -1, 0x01, 0xA1, -1, -1, -1, -1, 0x3D, 0x02, 0x10 });
        FIND("dart_2_sig", "client", {0x88, 0x1D, -1, -1, -1, -1, 0x88, 0x1D, -1, -1, -1, -1, 0x88, 0x1D, -1, -1, -1, -1, 0x8B, 0x5C, 0x24, 0x28, 0x83, 0xC4, 0x08, 0xE8});
        FIND("keyboard_keys_sig", "client", { 0xB8, -1, -1, -1, -1, 0xBA, 0x6D, 0x00, 0x00, 0x00, 0x8D, 0x49, 0x00, 0x80, -1, 0x6D, 0x01, 0x75, 0x05 });
        FIND("server_info_host_sig", "client", { 0xBF, -1, -1, -1, -1, 0xF3, 0xAB, 0xA1, -1, -1, -1, -1, 0xBA, -1, -1, -1, -1, 0xC7, 0x40, 0x08, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x66, 0x8B, 0x0D, -1, -1, -1, -1, 0x66, 0x89, 0x0D, -1, -1, -1, -1, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF });
        FIND("server_info_client_sig", "client", { 0xBA, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x66, 0xA1, -1, -1, -1, -1, 0x66, 0x25, 0xF9, 0xFF, 0x83, 0xCA, 0xFF, 0x66, 0xA3, -1, -1, -1, -1, 0xB9, 0x0C, 0x00, 0x00, 0x00 });
        FIND("console_enabled_sig", "client", {0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0x74, 0x28, 0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0x75, 0x1F, 0x57});
        FIND("draw_8_bit_text_sig", "client", { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x81, 0xEC, 0xA4, 0x00, 0x00, 0x00, 0x53, 0x8B, 0xD8, 0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0x56, 0x57, 0x0F, 0x84, 0xDA, 0x01, 0x00, 0x00 });
        FIND("draw_16_bit_text_sig", "client", { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x81, 0xEC, 0xA4, 0x00, 0x00, 0x00, 0x53, 0x8B, 0xD8, 0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0x56, 0x57, 0x8B, 0xF9, 0x0F, 0x84, 0xD8, 0x01, 0x00, 0x00 });
        FIND("text_hook_sig", "client", { 0xE8, -1, -1, -1, -1, 0xE8, 0x8F, 0xBB, 0x00, 0x00 });
        FIND("on_key_press_sig", "client", {0x89, 0x0C, 0x85, -1, -1, -1, -1, 0x66, 0xFF, 0x05, -1, -1, -1, -1, 0x59, 0xC3});
        FIND("key_press_mov_sig", "client", {0xC6, 0x80, -1, -1, -1, -1, 0x01, 0xEB, 0x21, 0x80, 0xB8, -1, -1, -1, -1, 0x01, 0x75, 0x11, 0x84, 0x1D, -1, -1, -1, -1, 0x75, 0x09, 0xC6, 0x80, -1, -1, -1, -1, 0x01, 0xEB, 0x07});
        FIND("rcon_message_sig", "client", {0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x08, 0x83, 0xC4, 0x58, 0xC3, 0x8B, 0xC2, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x58, 0xC3});
        FIND("player_name_sig", "client", { 0xBE, -1, -1, -1, -1, 0x8D, 0x7C, 0x24, 0x10, 0xF3, 0xA5, 0x8B, 0x42, 0x34 });
        FIND("load_keystone_sig", "client", { 0x74, 0x10, 0x5F, 0xC7, 0x05, -1, -1, -1, -1, 0x00, 0x00, 0x00, 0x00, 0x5E, 0x83, 0xC4, 0x10, 0xC3 });
        FIND("load_eula_sig", "client", { 0x8D, 0x55, 0xC8, 0x52, 0x68, 0x19, 0x00, 0x02, 0x00, 0x53 });
        FIND("registry_check_1_sig", "client", { 0x51, 0x6A, 0x00, 0x8D, 0x44, 0x24, 0x04, 0x50, 0x6A, 0x00, 0x68, 0x06, 0x00, 0x02, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00 });
        FIND("registry_check_2_sig", "client", { 0x81, 0xEC, 0x1C, 0x01, 0x00, 0x00, 0x53, 0x8D, 0x44, 0x24, 0x04, 0x50, 0x68, 0x19, 0x00, 0x02, 0x00, 0x6A, 0x00 });
        FIND("registry_check_3_sig", "client", {0x83, 0xEC, 0x08, 0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x85, 0xB7, 0x00, 0x00, 0x00});
        FIND("registry_check_4_sig", "client", {0x83, 0xEC, 0x0C, 0xA1, -1, -1, -1, -1, 0x56, 0x33, 0xF6, 0x85, 0xC0, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x44, 0x24, 0x0C, 0x04, 0x00, 0x00, 0x00});
        FIND("gamma_sig", "client", {0xA0, -1, -1, -1, -1, 0x83, 0xEC, 0x08, 0x84, 0xC0, 0x74, 0x0D, 0xA1});
        FIND("setup_master_server_connection_sig", "client", {0x6A, 0x00, 0x68, -1, -1, -1, -1, 0x6A, 0x01, 0x6A, 0x0A, 0x6A, 0x00, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1});
        FIND("console_fade_call_sig", "client", {0x75, 0x05, 0xE8, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x84, 0xC0, 0x74, 0x0D});
        FIND("console_fade_fn_sig", "client", {0x8B, 0x15, -1, -1, -1, -1, 0x83, 0xFA, 0xFF, 0x74, 0x43, 0x56, 0x8D, 0x64, 0x24, 0x00, 0x8B, 0x0D});
        FIND("death_timer_reset_sig", "client", {0x66, 0x40, 0x66, 0x83, 0xF9, 0x5A, 0x66, 0xA3, -1, -1, -1, -1, 0x7E, 0x12, 0x88, 0x1D, -1, -1, -1, -1, 0x66, 0x89, 0x1D, -1, -1, -1, -1, 0xE8});
        FIND("bsp_poly_count_sig", "client", { 0x66, 0xFF, 0x05, -1, -1, -1, -1, 0x8B, 0x47, 0x18, 0x83, 0xC6, 0x04, 0x45, 0x0F, 0xBF, 0xCD, 0x3B, 0xC8 });
        FIND("current_ip_string_sig", "client", { 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x20, 0xB8, -1, -1, -1, -1, 0xC3, 0x66, 0x83, 0xF9, 0x10 });
        FIND("on_connect_sig", "client", { 0x83, 0xEC, 0x08, 0x55, 0x8B, 0x6C, 0x24, 0x10, 0x56, 0x57, 0x8B, 0xC3 });

        FIND("bsp_poly_1_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0xE8, -1, -1, -1, -1, 0x66, 0x39, 0x1D, -1, -1, -1, -1, 0x0F, 0x85, 0xEE, 0x00, 0x00, 0x00 });
        FIND("bsp_poly_2_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x3A, 0xC3, 0x66, 0x89, 0x35 });
        FIND("bsp_poly_3_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x3A, 0xC3, 0x89, 0x1D });
        FIND("bsp_poly_4_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x3A, 0xC3, 0x0F, 0x84, 0x8A });
        FIND("bsp_poly_5_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x3A, 0xC3, 0x74, 0x5C, 0x8B, 0x0D, -1, -1, -1, -1, 0xA1 });
        FIND("bsp_poly_6_sig", "client", { 0xB9, -1, -1, -1, -1, 0x89, 0x1D, -1, -1, -1, -1, 0x88, 0x1D, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x3A, 0xC3, 0x74, 0x26, 0xE8 });
        FIND("bsp_poly_7_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0xA1, -1, -1, -1, -1, 0x3B, 0xC3, 0x74, 0x09, 0x8B, 0x40, 0x34, 0x3B, 0xC3, 0x74, 0x02, 0xFF, 0xD0, 0xE8 });
        FIND("bsp_poly_8_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x8B, 0x0D, -1, -1, -1, -1, 0xA1, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x53, 0x53 });
        FIND("bsp_poly_9_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0xE8, 0x3F, 0x77, 0x00, 0x00, 0xE8 });
        FIND("bsp_poly_10_sig", "client", { 0x68, -1, -1, -1, -1, 0xBB, -1, -1, -1, -1, 0xE8, 0x52, 0x05, 0x00, 0x00, 0x83, 0xC4, 0x04, 0x83, 0xF8, 0xFF, 0xA3 });
        FIND("bsp_poly_11_sig", "client", { 0xB9, -1, -1, -1, -1, 0xE8, 0x9B, 0x04, 0x00, 0x00, 0xA1, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x3D, 0x01, 0x01, 0xFF, 0xFF, 0x73, 0x15, 0xA1 });
        FIND("bsp_poly_12_sig", "client", { 0xBB, -1, -1, -1, -1, 0x83, 0xFF, 0xFF, 0x74, 0x24, 0x8B, 0x84, 0x24, 0x10, 0x40, 0x00, 0x00, 0xE8, -1, -1, -1, -1, 0x6A, 0x00, 0x6A, 0x00 });
        FIND("bsp_poly_13_sig", "client", { 0xBB, -1, -1, -1, -1, 0x83, 0xFF, 0xFF, 0x74, 0x24, 0x8B, 0x84, 0x24, 0x14, 0x40, 0x00, 0x00, 0xE8, -1, -1, -1, -1, 0x6A, 0x00, 0x6A, 0x00 });
        FIND("bsp_poly_limit_1_sig", "client", { 0x66, 0x81, 0x3D, -1, -1, -1, -1, 0x00, 0x40, 0x0F, 0x8D, 0x15, 0x01, 0x00, 0x00 });
        FIND("bsp_poly_limit_2_sig", "client", { 0x66, 0x81, 0x3D, -1, -1, -1, -1, 0x00, 0x40, 0x0F, 0x8D, 0x8F, 0x00, 0x00, 0x00 });
        FIND("bsp_poly_limit_3_sig", "client", { 0x66, 0x81, 0x3D, -1, -1, -1, -1, 0x00, 0x40, 0x7D, 0x19, 0x0B, 0xCA, 0x89, 0x08 });
        FIND("visible_object_list_1_sig", "client", { 0x8B, 0x04, 0x95, -1, -1, -1, -1, 0x8D, 0x7C, 0x24, 0x10, 0x89, 0x44, 0x24, 0x10, 0xE8, 0xE9, 0x01, 0x00, 0x00, 0x46 });
        FIND("visible_object_list_2_sig", "client", { 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x89, 0x0D, -1, -1, -1, -1, 0xC6, 0x40, 0x01, 0x01 });
        FIND("visible_object_list_3_sig", "client", { 0x8D, 0x04, 0x95, -1, -1, -1, -1, 0x50, 0xE8, -1, -1, -1, -1, 0x66, 0x01, 0x05 });
        FIND("visible_object_limit_1_sig", "client", { 0x66, 0x81, 0x3D, -1, -1, -1, -1, 0x00, 0x01, 0x75, 0x10, 0xA0 });
        FIND("visible_object_limit_2_sig", "client", { 0xB9, 0x00, 0x01, 0x00, 0x00, 0x2B, 0xCA, 0x68, -1, -1, -1, -1, 0x0F, 0xBF, 0xD0 });
        FIND("draw_distance_sig", "client", { 0xA1, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x89, 0x87, 0x98, 0x00, 0x00, 0x00, 0xA0 });
        FIND("visible_object_ptr_sig", "client", { 0x8B, 0x0C, 0x85, -1, -1, -1, -1, 0x89, 0x0F, 0xE8, 0x0F, 0x00, 0x00, 0x00, 0x46 });
        FIND("default_resolution_sig", "client", {0xC7, 0x44, 0x24, 0x20, 0x20, 0x03, 0x00, 0x00, 0xC7, 0x44, 0x24, 0x24, 0x58, 0x02, 0x00, 0x00, 0xC7, 0x44, 0x24, 0x28, 0x3C, 0x00, 0x00, 0x00});
        FIND("load_profile_resolution_sig", "client", {0x74, 0x43, 0x8D, 0x4C, 0x24, 0x38, 0x51, 0x8B, 0xC7});
        FIND("default_resolution_set_sig", "client", {0x52, 0x55, 0x50, 0xFF, 0x51, 0x40, 0x85, 0xC0});
        FIND("resolution_sig", "client", { 0x75, 0x0A, 0x66, 0xA1, -1, -1, -1, -1, 0x66, 0x89, 0x42, 0x04, 0x83, 0xC4, 0x10, 0xC3 });
        FIND("fix_fov_sig", "client", { 0xD8, 0x0D, -1, -1, -1, -1, 0xDD, 0x05, 0xC0, 0x21, 0x61, 0x00, 0xD9, 0xF3, 0xDC, 0xC0, 0xD9, 0x9F, 0x80, 0x00, 0x00, 0x00, 0x0F, 0x84, 0xDE, 0x00, 0x00, 0x00, 0xA0, -1, -1, -1, -1, 0x84, 0xC0 });
        FIND("flag_table_sig", "client", { 0x8B, 0x3D, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x8D, 0x4E, 0x01, 0x83, 0xCB, 0xFF, 0x66, 0x85, 0xC9, 0x7C, 0x31 });
        FIND("antenna_table_sig", "client", { 0x8B, 0x15, -1, -1, -1, -1, 0x8B, 0xC7, 0x25, 0xFF, 0xFF, 0x00, 0x00, 0xC1, 0xE0, 0x05, 0x55, 0x8B, 0x6C, 0x08, 0x14, 0x89, 0x6C, 0x24, 0x28 });
        FIND("light_table_sig", "client", { 0x8B, 0x0D, -1, -1, -1, -1, 0x8B, 0x51, 0x34, 0x56, 0x8B, 0xF0, 0x81, 0xE6, 0xFF, 0xFF, 0x00, 0x00, 0x6B, 0xF6, 0x7C });
        FIND("particle_table_sig", "client", { 0x8B,0x2D,-1,-1,-1,-1,0x83,0xCA,0xFF,0x8B,0xFD,0xE8,-1,-1,-1,-1,0x8B,0xF8,0x83,0xFF,0xFF,0x0F,0x84,0x10,0x06,0x00,0x00 });
        FIND("visible_object_count_sig", "client", { 0x66, 0x39, 0x35, -1, -1, -1, -1, 0x57, 0x8B, 0xF8, 0x7E, 0x1B, 0x0F, 0xBF, 0xC6 });
        FIND("followed_object_sig", "client", { 0x83, 0xC8, 0xFF, 0x66, 0xA3, -1, -1, -1, -1, 0xA3, -1, -1, -1, -1 });
        FIND("descope_fix_sig", "client", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x80, 0x7D, 0x20, 0x01, 0x0F, 0x85 });
        FIND("controls_sig", "client", {0x88, 0x85, -1, -1, -1, -1, 0x8A, 0x1D, -1, -1, -1, -1, 0x8B, 0x0C, 0xBD, -1, -1, -1, -1, 0x83, 0xF9, 0xFF});
        FIND("aim_assist_sig", "client", {0x84, 0xC0, 0x0F, 0x84, 0x6E, 0x03, 0x00, 0x00, 0x8A, 0x57, 0x0B});
        FIND("should_use_aim_assist_sig", "client", {0x88, 0x15, -1, -1, -1, -1, 0x8B, 0x56, 0x08, 0x89, 0x44, 0x24, 0x38, 0x89, 0x4C, 0x24, 0x3C, 0x89, 0x54, 0x24, 0x40, 0x33, 0xFF});
        FIND("aim_assist_enabled_sig", "client", {0xA0, -1, -1, -1, -1, 0x83, 0xC4, 0x10, 0x84, 0xC0, 0x0F, 0x84});
        FIND("bsp_poly_movsx_sig", "client", {0x0F, 0xBF, 0xC0, 0x53, 0x8B, 0xD9, 0x8D, 0x0C, 0x83, 0x56, 0x8B, 0x35});
        FIND("bsp_poly_movsx_2_sig", "client", {0x83, 0xC8, 0xFF, 0x66, 0x85, 0xFF, 0x55, 0x8B, 0x6C, 0x24, 0x08, 0x7E, 0x67, 0x0F, 0xBF, 0xD7});
        FIND("bsp_poly_another_limit_sig", "client", {0xB9, 0x00, 0x80, 0x00, 0x00, 0x56, 0x8B, 0x35});
        FIND("bsp_poly_bsp_render_calls_sig", "client", { 0xE8, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x33, 0xDB, 0x3A, 0xC3, 0x74, 0x28, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF });

        FIND("fix_counters_sig", "client", {0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x20, 0x5F, 0x5E, 0x5D, 0x5B, 0x81, 0xC4, 0xA8, 0x00, 0x00, 0x00, 0xC3});

        FIND("lens_scale_sig", "client_sun", { 0xD8, 0x0D, -1, -1, -1, -1, 0xD8, 0x0D, -1, -1, -1, -1, 0xD8, 0x25, -1, -1, -1, -1, 0xD9, 0x5C, 0x24, 0x08, 0xDB, 0x44, 0x24, 0x10, 0xD8, 0x7C, 0x24, 0x0C });

        FIND("model_lod_sig", "client_lod", { 0xBA, 0x04, 0x00, 0x00, 0x00, 0x89, 0x55, 0xF8, 0x8D, 0x49, 0x00, 0xD9, 0x45, 0x08, 0x0F, 0xBF, 0xC2 });

        FIND("diagonals_sig", "client_diagonals", {0xD8, 0x1D, -1, -1, -1, -1, 0xDF, 0xE0, 0xF6, 0xC4, 0x41, 0x75, 0x08, 0xDD, 0xD8, 0xD9, 0x05, -1, -1, -1, -1, 0xD9, 0x44, 0x24, 0x04, 0xD8, 0x1D, -1, -1, -1, -1, 0xDF, 0xE0});

        FIND("auto_center_sig", "client_auto_center", { 0x7A, 0x0B, 0x8A, 0x47, 0x26, 0x84, 0xC0 });

        FIND("fp_cam_tick_rate_sig", "client_interpolate", { 0xD8, 0x0D, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0xD8, 0x15, -1, -1, -1, -1, 0xDF, 0xE0, 0xF6, 0xC4, 0x41, 0x75, 0x0A, 0xDD, 0xD8, 0xD9, 0x05 });
        FIND("fp_interp_sig", "client_interpolate", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x10, 0x5F, 0x5E, 0x5B, 0x83, 0xC4, 0x18, 0xC3 });

        FIND("widescreen_scope_sig", "client_widescreen", { 0xC7, 0x44, 0x24, 0x38, 0x00, 0x00, 0x20, 0x44, 0xC7, 0x44, 0x24, 0x3C, 0x00, 0x00, 0xF0, 0x43, 0x7D, 0x06 });
        FIND("widescreen_element_scaling_sig", "client_widescreen", { 0xC7, 0x84, 0x24, 0xA8, 0x00, 0x00, 0x00, 0xCD, 0xCC, 0x4C, 0x3B, 0xC7, 0x84, 0x24, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x84, 0x24, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x84, 0x24, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x84, 0x24, 0xBC, 0x00, 0x00, 0x00, 0x89, 0x88, 0x88, 0xBB });
        FIND("widescreen_element_position_hud_sig", "client_widescreen", { 0xE8, 0xD3, 0xFD, 0x06, 0x00, 0x83, 0xC4, 0x04, 0x5F, 0x5D, 0x5B, 0x81, 0xC4, 0x10, 0x01, 0x00, 0x00, 0xC3 });
        FIND("widescreen_element_position_multitexture_overlay_sig", "client_widescreen", { 0xE8, 0x2E, 0xF4, 0x06, 0x00, 0x83, 0xC4, 0x04, 0x5F, 0x5E, 0x5D, 0x5B, 0x81, 0xC4, 0x94, 0x01, 0x00, 0x00, 0xC3 });
        FIND("widescreen_element_position_motion_sensor_sig", "client_widescreen", { 0xE8, 0x68, 0x33, 0x00, 0x00, 0x83, 0xC4, 0x08, 0x8B, 0x7C, 0x24, 0x18, 0x8B, 0x0D, 0x24, 0xCD, 0x68, 0x00, 0x33, 0xC0 });
        FIND("widescreen_element_position_menu_sig", "client_widescreen", { 0xE8, 0x1D, 0x43, 0x08, 0x00, 0x83, 0xC4, 0x04, 0x5F, 0x5D, 0x5B, 0x5E, 0x81, 0xC4, 0x30, 0x01, 0x00, 0x00, 0xC3 });
        FIND("widescreen_element_position_letterbox_sig", "client_widescreen", { 0xE8, 0xE8, 0x5D, 0x0D, 0x00, 0x83, 0xC4, 0x04, 0x66, 0x89, 0x1D, 0xEA, 0x7C, 0x63, 0x00, 0x5F, 0x5E, 0x5B, 0x8B, 0xE5, 0x5D, 0xC3 });
        FIND("widescreen_text_scaling_sig", "client_widescreen", { 0xC7, 0x05, 0xC8, 0xCC, 0x67, 0x00, 0xCD, 0xCC, 0x4C, 0x3B });
        FIND("widescreen_console_input_sig", "client_widescreen", { 0x81, 0xC1, 0x80, 0x02, 0x00, 0x00, 0x89, 0x15, 0xEC, 0xF6, 0x67, 0x00, 0x8D, 0x85, 0x84, 0x00, 0x00, 0x00, 0x66, 0x89, 0x4C, 0x24, 0x16, 0x8B, 0x08, 0x89, 0x0D });
        FIND("widescreen_menu_text_sig", "client_widescreen", { 0xE8, 0x78, 0x9E, 0x07, 0x00, 0x83, 0xC4, 0x0C, 0x5F, 0x5E, 0x5D, 0x5B, 0x83, 0xC4, 0x7C, 0xC3 });
        FIND("widescreen_menu_text_2_sig", "client_widescreen", { 0xE8, 0x42, 0x99, 0x07, 0x00, 0x8B, 0x44, 0x24, 0x2C, 0x83, 0xC4, 0x18, 0x83, 0xBD, 0xF8, 0x00, 0x00, 0x00, 0xFF });
        FIND("widescreen_text_max_x_sig", "client_widescreen", { 0xBA, 0x80, 0x02, 0x00, 0x00, 0x66, 0x8B, 0x03, 0x66, 0x85, 0xC0, 0x7D, 0x04, 0x33, 0xC9, 0xEB, 0x03, 0x0F, 0xBF, 0xC8 });
        FIND("widescreen_text_f1_sig", "client_widescreen", { 0xE8, 0xE7, 0x1E, 0x0B, 0x00, 0x83, 0xC4, 0x0C, 0x5F, 0x5E, 0x66, 0x89, 0x1D, 0x08, 0xF7, 0x67, 0x00, 0x5B, 0x83, 0xC4, 0x28, 0xC3 });
        FIND("widescreen_text_pgcr_sig", "client_widescreen", { 0xE8, 0xCD, 0xA2, 0x0B, 0x00, 0x83, 0xC4, 0x14, 0xC3 });
        FIND("widescreen_element_motion_sensor_scaling_sig", "client_widescreen", { 0xC7, 0x44, 0x24, 0x78, 0xCD, 0xCC, 0x4C, 0x3B, 0xC7, 0x44, 0x24, 0x7C, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x84, 0x24, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x84, 0x24, 0x84, 0x00, 0x00, 0x00, 0x33, 0x33 });
        FIND("widescreen_text_stare_name_sig", "client_widescreen", { 0xE8, 0x1D, 0x58, 0x0B, 0x00, 0x66, 0xC7, 0x05, 0xF4, 0xF6, 0x67, 0x00, 0xFF, 0xFF, 0x66, 0xC7, 0x05, 0xF6, 0xF6, 0x67, 0x00, 0x00, 0x00 });
        FIND("widescreen_text_f3_name_sig", "client_widescreen", { 0xE8, 0x3A, 0xAB, 0x06, 0x00, 0x83, 0xC4, 0x24, 0x5B, 0x5F, 0x5E, 0x5D, 0x81, 0xC4, 0xB4, 0x00, 0x00, 0x00, 0xC3 });
        FIND("widescreen_nav_marker_sig", "client_widescreen", { 0xE8, 0x67, 0xEF, 0x04, 0x00, 0x83, 0xC4, 0x10, 0x8B, 0x54, 0x24, 0x18, 0x42, 0x83, 0xC3, 0x20, 0x81, 0xFB, 0x60, 0xCC, 0x68, 0x00 });
        FIND("widescreen_nav_marker_sp_sig", "client_widescreen", { 0xE8, 0xEE, 0xF8, 0xFF, 0xFF, 0x83, 0xC4, 0x10, 0x8B, 0x4C, 0x24, 0x10, 0x8B, 0x44, 0x24, 0x14, 0x40, 0x66, 0x3D, 0x04, 0x00 });
        FIND("widescreen_text_f2_text_position_heading_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x22, 0x76, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x1C, 0x41, 0x00, 0x66, 0x89, 0x7C, 0x24, 0x20, 0xE8, 0x65, 0x18, 0x0F, 0x00, 0x8D, 0x8C, 0x24, 0x54, 0x04, 0x00, 0x00, 0x51 });
        FIND("widescreen_text_f2_text_position_motd_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x3E, 0x69, 0x00, 0x66, 0xC7, 0x44, 0x24, 0x42, 0x71, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x3C, 0x5E, 0x01, 0x89, 0x35, 0xF8, 0xF6, 0x67, 0x00 });
        FIND("widescreen_text_f2_text_position_motd_body_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x3E, 0x69, 0x00, 0x66, 0xC7, 0x44, 0x24, 0x42, 0x71, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x3C, 0x77, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x40, 0xD1, 0x01 });
        FIND("widescreen_text_f2_text_position_rules_1_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x26, 0x4A, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x2A, 0x76, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x28, 0xC2, 0x01, 0x66, 0x89, 0x5C, 0x24 });
        FIND("widescreen_text_f2_text_position_rules_2_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x16, 0x0A, 0x00, 0x66, 0xC7, 0x44, 0x24, 0x1A, 0x36, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x18, 0xC2, 0x01, 0x66, 0x89, 0x44, 0x24, 0x14, 0xE8, 0x6D, 0xB1, 0xFD, 0xFF });
        FIND("widescreen_text_f2_text_position_rules_3_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x12, 0x4A, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x16, 0x76, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x14, 0xC2, 0x01, 0x66, 0x89, 0x5C, 0x24, 0x10, 0x0F, 0x84, 0x6F, 0x01, 0x00, 0x00, 0xA0 });
        FIND("widescreen_text_f2_text_position_rules_4_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x3A, 0x36, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x38, 0xC2, 0x01, 0x66, 0x89, 0x7C, 0x24, 0x34, 0xE8, 0x25, 0x18, 0x0F, 0x00, 0x8B, 0x44, 0x24, 0x68 });
        FIND("widescreen_text_f2_text_position_rules_4_left_x_sig", "client_widescreen", { 0xBB, 0x0A, 0x00, 0x00, 0x00, 0xBF, 0x55, 0x00, 0x00, 0x00, 0x6A, 0x00, 0x8D, 0x44, 0x24, 0x34, 0x8B, 0xCE });
        FIND("widescreen_teammate_indicator_sig", "client_widescreen", { 0xE8, 0xB3, 0x83, 0x08, 0x00, 0x83, 0xC4, 0x04, 0x5F, 0x5E, 0x5D, 0x5B, 0x81, 0xC4, 0x14, 0x01, 0x00, 0x00, 0xC3 });
        FIND("widescreen_hud_pickup_icon_sig", "client_widescreen", { 0xE8, 0x33, 0xF1, 0xFF, 0xFF, 0x8A, 0x46, 0x0D, 0x83, 0xC4, 0x14, 0xA8, 0x04, 0x74, 0x2F, 0x0F, 0xBF, 0x4E, 0x02 });
        FIND("widescreen_text_loading_screen_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x10, 0x9A, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x16, 0x80, 0x02, 0x66, 0x89, 0x7C, 0x24, 0x14, 0x0F, 0x87, 0xD2, 0x01, 0x00, 0x00 });
        FIND("widescreen_screen_effect_sig", "client_widescreen", { 0xE8, 0x08, 0x91, 0x09, 0x00, 0xEB, 0x07, 0x33, 0xC0, 0xE8, 0x1F, 0x9C, 0x09, 0x00, 0xE8, 0x8A, 0x51, 0x01, 0x00, 0xA1, 0x24, 0xCD, 0x68, 0x00, 0x85, 0xC0, 0x5E });
        FIND("widescreen_text_f1_server_name_position_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x3A, 0x7B, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x38, 0xCC, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x34, 0xB8, 0x01, 0x7E, 0x08, 0x8B, 0x75, 0x64, 0x83, 0xFE, 0xFF, 0x75, 0x03, 0x8B, 0x75, 0x54, 0x57 });
        FIND("widescreen_text_f1_server_ip_position_sig", "client_widescreen", { 0x66, 0xC7, 0x44, 0x24, 0x3A, 0x7B, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x38, 0xE0, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x34, 0xCC, 0x01, 0x7E, 0x08, 0x8B, 0x41, 0x64, 0x83, 0xF8, 0xFF, 0x75, 0x03, 0x8B, 0x41, 0x54 });
        FIND("widescreen_console_tabs_sig", "client_widescreen", { 0x66, 0x8B, 0x15, -1, -1, -1, -1, 0xD8, 0x4C, 0x24, 0x1C, 0xA1, -1, -1, -1, -1, 0x33, 0xC9, 0x66, 0x8B, 0x0D, -1, -1, -1, -1, 0xD9, 0x5C, 0x24, 0x1C, 0x66, 0xF7, 0xD9, 0x66, 0x03, 0xD1, 0xF7, 0xD8, 0x66, 0x8B, 0xEF, 0x2B, 0xFB, 0x66, 0x03, 0xE8, 0x66, 0x89, 0x54, 0x24, 0x12, 0x8D, 0x14, 0x38, 0x8A, 0x46, 0x0C, 0x81, 0xC1, 0x80, 0x02, 0x00, 0x00, 0x84, 0xC0, 0x66, 0x89, 0x4C, 0x24, 0x16, 0x66, 0x89, 0x54, 0x24, 0x10, 0x66, 0x89, 0x6C, 0x24, 0x14, 0x74, 0x21, 0xA1, -1, -1, -1, -1, 0x66, 0x8B, 0x0D, -1, -1, -1, -1 });
        FIND("widescreen_cinematic_text_sig", "client_widescreen", { 0xE8, 0xA6, 0xDB, 0x0C, 0x00, 0xA1, 0x70, 0xCD, 0x68, 0x00, 0x8A, 0x48, 0x02, 0x83, 0xC4, 0x0C, 0x84, 0xC9, 0xC7, 0x05, 0xDC, 0x83, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x04, 0x33, 0xC0, 0xEB, 0x04 });
        FIND("widescreen_input_text_sig", "client_widescreen", { 0xE8, 0x3D, 0xBF, 0x06, 0x00, 0xA0, 0x47, 0x44, 0x6B, 0x00, 0x83, 0xC4, 0x0C, 0x84, 0xC0, 0x0F, 0x85, 0xEE, 0x00, 0x00, 0x00, 0x83, 0x3D, 0x54, 0x44, 0x6B, 0x00, 0xFF, 0x0F, 0x84, 0xE1, 0x00, 0x00, 0x00 });
        FIND("widescreen_mouse_sig", "client_widescreen", { 0x7D, 0x08, 0x89, 0x15, -1, -1, -1, -1, 0xEB, 0x16, 0x3D, 0x80, 0x02, 0x00, 0x00 });

        FIND("letterbox_sig", "client_letterbox", { 0x8B, 0x15, -1, -1, -1, -1, 0x8A, 0x4A, 0x08, 0x83, 0xEC, 0x34, 0x84, 0xC9, 0x53, 0x55, 0x56, 0x57, 0x75, 0x13 });

        FIND("client_drm_sig", "client_drm", { -1, -1, 0x75, 0x11, 0x6A, 0x01, 0x6A, 0x7E, 0x68, 0xA0, 0x00, 0x00, 0x00 });

        FIND("uncap_cinematic_sig", "client_interpolate", { 0x74, 0x04, 0xB3, 0x01, 0xEB, 0x02, 0x32, 0xDB, 0x8B, 0x2D });

        FIND("kill_feed_sig", "client_custom_chat", {0x81, 0xEC, 0x00, 0x08, 0x00, 0x00, 0x83, 0xFF, 0xFF, 0x55, 0x8B, 0xAC, 0x24, 0x0C, 0x08, 0x00, 0x00, 0x0F, 0x84});
        FIND("chat_open_sig", "client_custom_chat", { 0xA0, -1, -1, -1, -1, 0x83, 0xEC, 0x08, 0x53, 0x33, 0xDB, 0x3A, 0xC3, 0x0F, 0x85, 0xCE, 0x01, 0x00, 0x00 });
        FIND("multiplayer_message_sig", "client_custom_chat", { 0xA1, -1, -1, -1, -1, 0x83, 0xEC, 0x08, 0x83, 0xF8, 0x08, 0x7C, 0x05, 0xE8 });
        FIND("on_multiplayer_chat_sig", "client_custom_chat", {0xE8, -1, -1, -1, -1, 0x84, 0xC0, 0x0F, 0x84, 0xE6, 0x01, 0x00, 0x00, 0x8A, 0x44, 0x24, 0x10, 0x3C, 0xFF, 0x0F, 0x84, 0x05, 0x01, 0x00, 0x00});
        FIND("chat_out_sig", "client_custom_chat", { 0x83, 0xEC, 0x10, 0x8A, 0x4C, 0x24, 0x14, 0x55, 0x6A, 0x00, 0x6A, 0x01, 0x6A, 0x00, 0x88, 0x4C, 0x24, 0x18 });

        FIND("af_is_enabled_sig", "client_af", { 0xA1, -1, -1, -1, -1, 0x85, 0xC0, 0xBE, 0x02, 0x00, 0x00, 0x00, 0x8B, 0xFE });
        FIND("block_camera_shake_sig", "client_camera_shake", { 0x0F, 0x85, 0xD1, 0x00, 0x00, 0x00, 0x8B, 0x15 });
        FIND("disable_buffering_sig", "client_disable_buffering", { 0xA1, -1, -1, -1, -1, 0x85, 0xC0, 0x74, 0x74, 0xA1, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x08, 0x52 });
        FIND("team_icon_background_name_sig", "client_gametype_indicator", { 0x68, -1, -1, -1, -1, 0xBF, 0x6D, 0x74, 0x69, 0x62, 0x89, 0x74, 0x24, 0x14, 0xE8, -1, -1, -1, -1, 0x6A, 0x00, 0x33, 0xFF });
        FIND("gametype_rules_sig", "client_gametype_rules", { 0xC7, -1, -1, -1, -1, 0x00, 0x96, 0x00, 0x00, 0x00, 0x5B, 0xC3 });
        FIND("hold_f1_sig", "client_hold_f1", { 0x81, 0x7A, 0x0C, 0xC2, 0x01, 0x00, 0x00, 0x7D, 0x33, 0x8B, 0x45, 0x74, 0x3B, 0xC7, 0x74, 0x16 });
        FIND("hud_kill_feed_sig", "client_hud_kill_feed", { 0xE8, -1, -1, -1, -1, 0xEB, 0x0E, 0x66, 0x8B, 0x46, 0x02, 0x8D, 0x54, 0x24, 0x0C, 0x52 });
        FIND("hud_kill_feed_host_kill_sig", "client_hud_kill_feed", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x66, 0x83, 0x3D, -1, -1, -1, -1, 0x02, 0x0F, 0x85, -1, -1, -1, -1, 0x55, 0x6A, 0x08, 0x8B, 0xCF });
        FIND("hud_kill_feed_host_betray_sig", "client_hud_kill_feed", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x66, 0x83, 0x3D, -1, -1, -1, -1, 0x02, 0x0F, 0x85, -1, -1, -1, -1, 0x55, 0x6A, 0x08, 0x8B, 0xCF, 0x8B, 0xC7, 0xE8 });

        FIND("loading_screen_singleplayer_sig", "client_loading_screen", { 0xC7, 0x05, -1, -1, -1, -1, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3D, -1, -1, -1, -1, 0x01 });
        FIND("loading_screen_host_sig", "client_loading_screen", { 0xC7, 0x05, -1, -1, -1, -1, 0x08, 0x00, 0x00, 0x00, 0x33, 0xC0, 0x88, 0x83, 0x22, 0x0F, 0x00, 0x00 });
        FIND("loading_screen_join_sig", "client_loading_screen", { 0xC7, 0x05, -1, -1, -1, -1, 0x08, 0x00, 0x00, 0x00, 0x8D, 0x83, 0x14, 0x0B, 0x00, 0x00, 0x8B, 0xF0 });

        FIND("mouse_accel_1_sig", "client_mouse_acceleration", { 0x7E, 0x0F, 0x83, 0xC1, 0x10, 0x40, 0x81, 0xF9, -1, -1, -1, -1, 0x7C, 0xF0, 0x5B, 0x59, 0xC3, 0xDD, 0xD8, 0xC1, 0xE0, 0x04 });
        FIND("mouse_accel_2_sig", "client_mouse_acceleration", { 0xD9, 0x80, -1, -1, -1, -1, 0xD8, 0xA0, -1, -1, -1, -1, 0xDE, 0xC9, 0xD8, 0x80, -1, -1, -1, -1, 0xD9, 0x44, 0x24, 0x0C, 0xD8, 0x88, -1, -1, -1, -1, 0xD8, 0x05, -1, -1, -1, -1, 0xDE, 0xC9, 0xDA, 0x4C, 0x24, 0x10, 0x5B, 0x59, 0xC3 });
        FIND("mouse_horiz_1_sig", "client_mouse_acceleration", { 0x8B, 0x0D, -1, -1, -1, -1, 0x50, 0x51, 0xE8, 0x13, 0xF0, 0xFF, 0xFF, 0xD9, 0xE0 });
        FIND("mouse_horiz_2_sig", "client_mouse_acceleration", { 0xA1, -1, -1, -1, -1, 0x50, 0xE8, 0x3A, 0xF0, 0xFF, 0xFF });
        FIND("mouse_vert_1_sig", "client_mouse_acceleration", { 0x8B, 0x0D, -1, -1, -1, -1, 0x50, 0x51, 0xE8, 0x8C, 0xF0, 0xFF, 0xFF, 0xD9, 0x1D });
        FIND("mouse_vert_2_sig", "client_mouse_acceleration", { 0x8B, 0x15, -1, -1, -1, -1, 0x50, 0x52, 0xE8, 0x62, 0xF0, 0xFF, 0xFF, 0xD9, 0xE0 });

        FIND("mouse_horiz_1_sig", "client_mouse_sensitivity", { 0x8B, 0x0D, -1, -1, -1, -1, 0x50, 0x51, 0xE8, 0x13, 0xF0, 0xFF, 0xFF, 0xD9, 0xE0 });
        FIND("mouse_horiz_2_sig", "client_mouse_sensitivity", { 0xA1, -1, -1, -1, -1, 0x50, 0xE8, 0x3A, 0xF0, 0xFF, 0xFF });
        FIND("mouse_vert_1_sig", "client_mouse_sensitivity", { 0x8B, 0x0D, -1, -1, -1, -1, 0x50, 0x51, 0xE8, 0x8C, 0xF0, 0xFF, 0xFF, 0xD9, 0x1D });
        FIND("mouse_vert_2_sig", "client_mouse_sensitivity", { 0x8B, 0x15, -1, -1, -1, -1, 0x50, 0x52, 0xE8, 0x62, 0xF0, 0xFF, 0xFF, 0xD9, 0xE0 });

        FIND("zoom_blur_1_sig", "client_zoom_blur", {0xD9, 0x42, 0x48, 0xD9, 0x42, 0x44, 0xDA, 0xE9, 0xDF, 0xE0, 0xF6, 0xC4, 0x44});
        FIND("zoom_blur_2_sig", "client_zoom_blur", {0xD9, 0x42, 0x50, 0xD8, 0x15, -1, -1, -1, -1, 0xDF, 0xE0});
        FIND("zoom_blur_3_sig", "client_zoom_blur", {0xF6, 0xC4, 0x41, 0x75, 0x0D, 0xD9, 0x5C, 0x24, 0x18, 0x66, 0xC7, 0x44, 0x24});
        FIND("zoom_blur_4_sig", "client_zoom_blur", {0xBD, 0x02, 0x00, 0x00, 0x00, 0x89, 0x44, 0x24, 0x18, 0x0F, 0xBF, 0xC5, 0x8D});

        FIND("ammo_counter_ss_sig", "client_split_screen_hud", {0x66, 0x8B, 0x40, 0x0C, 0x33, 0xC9, 0x66, 0x83, 0x7A, 0x3C, 0x02});
        FIND("hud_text_ss_sig", "client_split_screen_hud", {0x66, 0x8B, 0x47, 0x0C, 0x66, 0x3D, 0x01, 0x00, 0x8B, 0x35});
        FIND("split_screen_hud_ss_sig", "client_split_screen_hud", {0x66, 0x83, 0x78, 0x0C, 0x01, 0x8B, 0x83, 0xA8, 0x02, 0x00, 0x00});

        FIND("console_prompt_color_sig", "client_console_prompt_color", {0xA3, -1, -1, -1, -1, 0xA1, -1, -1, -1, -1, 0xA3, -1, -1, -1, -1, 0xA0, -1, -1, -1, -1, 0x89, 0x0D, -1, -1, -1, -1, 0x8B, 0x0D, -1, -1, -1, -1, 0xA2, -1, -1, -1, -1, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0x53});

        FIND("ss_elements_sig", "client_score_screen", { 0x8B, 0x0D, -1, -1, -1, -1, 0x89, 0x44, 0x24, 0x10, 0xA1, -1, -1, -1, -1, 0x89, 0x4C, 0x24, 0x0C, 0x8B, 0x88, 0x40, 0x01, 0x00, 0x00, 0x33, 0xDB, 0x3B, 0xCB, 0x74, 0x08, 0x8B, 0x80, 0x44, 0x01, 0x00, 0x00, 0xEB, 0x02, 0x33, 0xC0, 0x3B, 0xD3, 0x8B, 0x70, 0x1C, 0x66, 0xC7, 0x44, 0x24, 0x14, 0x19, 0x00, 0x66, 0xC7, 0x44, 0x24, 0x16, 0x5A, 0x00, 0x66, 0xC7, 0x44, 0x24, 0x18, 0x2C, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x1A, 0x6D, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x1C, 0xAE, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x1E, 0xEF, 0x01, 0x66, 0xB8, 0x30, 0x02, 0x74, 0x2F });
        FIND("ss_score_position_sig", "client_score_screen", { 0x6B, 0xD2, 0x0F, 0x8B, 0x4C, 0x24, 0x2C, 0x83, 0xC2, 0x3B, 0x66, 0x89, 0x54, 0x24, 0x0C });
        FIND("ss_score_background_sig", "client_score_screen", { 0xC7, 0x44, 0x24, 0x24, 0x00, 0x00, 0x00, 0x3E, 0xC7, 0x44, 0x24, 0x28, 0x00, 0x00, 0x00, 0x3E, 0xC7, 0x44, 0x24, 0x2C, 0x00, 0x00, 0x00, 0x3E, 0x66, 0xC7, 0x44, 0x24, 0x3E, 0x0A, 0x00, 0x66, 0xC7, 0x44, 0x24, 0x42, 0x76, 0x02, 0x66, 0xC7, 0x44, 0x24, 0x40, 0x86, 0x01, 0x66, 0xC7, 0x44, 0x24, 0x3C, 0x3C, 0x00, 0xE8, -1, -1, -1, -1, 0x8D });

        FIND("join_server_ip_text_sig", "client_server_ip", { 0xB8, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x8B, 0x84, 0x24, 0x28, 0x01, 0x00, 0x00, 0x50 });
        FIND("f1_ip_text_render_call_sig", "client_server_ip", { 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x5F, 0x5E, 0x5D, 0x5B, 0x81, 0xC4, 0xE0, 0x06, 0x00, 0x00, 0xC3 });
        FIND("create_server_ip_text_sig", "client_server_ip", { 0x8B, 0x0D, -1, -1, -1, -1, 0x8B, 0xC1, 0xC1, 0xE0, 0x10, 0x81, 0xE1, 0x00, 0xFF, 0x00, 0x00, 0x0B, 0xC1, 0x66, 0x8B, 0x0D, -1, -1, -1, -1, 0x33, 0xD2, 0x8A, 0xF1, 0xC1, 0xE0, 0x08, 0x0F, 0xB6, 0xCD, 0x0B, 0xC2, 0x0B, 0xC1, 0x50, 0xFF, 0x15, -1, -1, -1, -1, 0x8B, 0xC8, 0x8D, 0x70, 0x01, 0x8D, 0x49, 0x00, 0x8A, 0x10 });

        FIND("analog_input_sig", "client_deadzones", { 0x66, 0x89, 0x11, 0x8B, 0x93, 0x34, 0x02, 0x00, 0x00, 0x40, 0x83, 0xC1, 0x02 });
        FIND("control_bitmask_sig", "client_auto_uncrouch", { 0x8D, 0x44, 0x0C, 0x48, 0x8A, 0x1C, 0x06, 0x88, 0x18, 0x8B, 0x5C, 0x24, 0x18, 0x41 });

        return signatures;
    }
}
