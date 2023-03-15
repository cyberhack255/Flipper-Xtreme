#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexWiiMenu,
    VarItemListIndexApp,
    VarItemListIndexRemoveApp,
    VarItemListIndexAddApp,
};

void xtreme_app_scene_mainmenu_var_item_list_callback(
    void* context,
    uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_mainmenu_wii_menu_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->wii_menu = value;
    app->save_settings = true;
}

static void xtreme_app_scene_mainmenu_app_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->mainmenu_app_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, *CharList_get(app->mainmenu_apps_names, app->mainmenu_app_index));
}

void xtreme_app_scene_mainmenu_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Wii menu", 2, xtreme_app_scene_mainmenu_wii_menu_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->wii_menu);
    variable_item_set_current_value_text(item, xtreme_settings->wii_menu ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "App",
        CharList_size(app->mainmenu_apps_names),
        xtreme_app_scene_mainmenu_app_changed,
        app);
    app->mainmenu_app_index = 0;
    variable_item_set_current_value_index(item, app->mainmenu_app_index);
    if(CharList_size(app->mainmenu_apps_names)) {
        variable_item_set_current_value_text(item, *CharList_get(app->mainmenu_apps_names, app->mainmenu_app_index));
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Remove App", 0, NULL, app);

    variable_item_list_add(var_item_list, "Add App", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_mainmenu_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(
            app->scene_manager, XtremeAppSceneMainmenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_mainmenu_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, XtremeAppSceneMainmenu, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexRemoveApp:
            if(!CharList_size(app->mainmenu_apps_names)) break;
            if(!CharList_size(app->mainmenu_apps_paths)) break;
            CharList_remove_v(app->mainmenu_apps_names, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            CharList_remove_v(app->mainmenu_apps_paths, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            app->save_mainmenu_apps = true;
            app->require_reboot = true;
            scene_manager_previous_scene(app->scene_manager);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMainmenu);
            break;
        case VarItemListIndexAddApp:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMainmenuAdd);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_mainmenu_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
