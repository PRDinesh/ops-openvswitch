/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Nicira, Inc.
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
 */

#ifndef DIRS_H
#define DIRS_H 1

const char *ovs_sysconfdir(void); /* /usr/local/etc */
const char *ovs_pkgdatadir(void); /* /usr/local/share/openvswitch */
const char *ovs_rundir(void);     /* /usr/local/var/run/openvswitch */
const char *ovs_logdir(void);     /* /usr/local/var/log/openvswitch */
const char *ovs_dbdir(void);      /* /usr/local/etc/openvswitch */
const char *ovs_bindir(void);     /* /usr/local/bin */
const char *ovs_pluginsdir(void); /* /usr/local/lib/openvswitch/plugins */

#endif /* dirs.h */
