/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Hewlett-Packard Company Confidential (C) Copyright 2015 Hewlett-Packard Development Company, L.P.
 */

#include <config.h>
#include <errno.h>
#include <ltdl.h>
#include <unistd.h>
#include "plugins.h"
#include "coverage.h"
#include "dynamic-string.h"
#include "openvswitch/vlog.h"
#include "lib/dirs.h"

VLOG_DEFINE_THIS_MODULE(plugins);

typedef void(*plugin_func)(void);
struct plugin_class {
    plugin_func init;
    plugin_func run;
    plugin_func wait;
    plugin_func destroy;
    plugin_func netdev_register;
    plugin_func ofproto_register;
    plugin_func bufmon_register;
};

static lt_dlinterface_id interface_id;

static int
plugins_open_plugin(const char *filename, void *data)
{
    struct plugin_class *plcl;
    lt_dlhandle handle;

    if (!(handle = lt_dlopenadvise(filename, *(lt_dladvise *)data))) {
        VLOG_ERR("Failed loading %s: %s", filename, lt_dlerror());
        return 0;
    }

    if (!(plcl = (struct plugin_class *)malloc(sizeof(struct plugin_class)))) {
        VLOG_ERR("Couldn't allocate plugin class");
        goto err_plugin_class;
    }

    if (!(plcl->init = lt_dlsym(handle, "init")) ||
        !(plcl->run = lt_dlsym(handle, "run")) ||
        !(plcl->wait = lt_dlsym(handle, "wait")) ||
        !(plcl->destroy = lt_dlsym(handle, "destroy"))) {
            VLOG_ERR("Couldn't initialize the interface for %s", filename);
            goto err_dlsym;
    }

    // The following APIs are optional, so don't fail if they are missing.
    plcl->netdev_register = lt_dlsym(handle, "netdev_register");
    plcl->ofproto_register = lt_dlsym(handle, "ofproto_register");
    plcl->bufmon_register = lt_dlsym(handle, "bufmon_register");


    if (lt_dlcaller_set_data(interface_id, handle, plcl)) {
        VLOG_ERR("plugin %s initialized twice? must be a bug", filename);
        goto err_set_data;
    }

    plcl->init();

    VLOG_INFO("Loaded plugin library %s", filename);
    return 0;

err_set_data:
err_dlsym:
    free(plcl);

err_plugin_class:
    if (lt_dlclose(handle)) {
        VLOG_ERR("Couldn't dlclose %s", filename);
    }

    return 0;
}

void
plugins_init(const char *path)
{
    char *plugins_path;
    lt_dladvise advise;

    if (path && !strcmp(path, "none")) {
        return;
    }

    if (!(plugins_path = path ? xstrdup(path) : xstrdup(ovs_pluginsdir()))) {
        VLOG_ERR("Failed to allocate plugins path");
        return;
    }

    if (lt_dlinit() ||
        lt_dlsetsearchpath(plugins_path) ||
        lt_dladvise_init(&advise)) {
        VLOG_ERR("ltdl initializations: %s", lt_dlerror());
        goto err_init;
    }

    if (!(interface_id = lt_dlinterface_register("ovs-plugin", NULL))) {
        VLOG_ERR("lt_dlinterface_register: %s", lt_dlerror());
        goto err_interface_register;
    }

    if (lt_dladvise_global(&advise) || lt_dladvise_ext (&advise) ||
        lt_dlforeachfile(lt_dlgetsearchpath(), &plugins_open_plugin, &advise)) {
        VLOG_ERR("ltdl setting advise: %s", lt_dlerror());
        goto err_set_advise;
    }

    VLOG_INFO("Successfully initialized all plugins");
    return;

err_set_advise:
    lt_dlinterface_free(interface_id);

err_interface_register:
    if (lt_dladvise_destroy(&advise)) {
        VLOG_ERR("destroying ltdl advise%s", lt_dlerror());
        return;
    }

err_init:
    free(plugins_path);
}

#define PLUGINS_CALL(FUNC) \
do { \
    lt_dlhandle iter_handle = 0; \
    struct plugin_class *plcl; \
    while ((iter_handle = lt_dlhandle_iterate(interface_id, iter_handle))) { \
        plcl = (struct plugin_class *)lt_dlcaller_get_data(interface_id, iter_handle); \
        if (plcl && plcl->FUNC) { \
            plcl->FUNC(); \
        } \
    } \
}while(0)

void
plugins_run(void)
{
    PLUGINS_CALL(run);
}

void
plugins_wait(void)
{
    PLUGINS_CALL(wait);
}

void
plugins_destroy(void)
{
    PLUGINS_CALL(destroy);
    lt_dlinterface_free(interface_id);
    lt_dlexit();
    VLOG_INFO("Destroyed all plugins");
}

void
plugins_netdev_register(void)
{
    PLUGINS_CALL(netdev_register);
}

void
plugins_ofproto_register(void)
{
    PLUGINS_CALL(ofproto_register);
}

void
plugins_bufmon_register(void)
{
    PLUGINS_CALL(bufmon_register);
}
