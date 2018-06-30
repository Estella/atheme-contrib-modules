/*
 * Copyright (c) 2005-2007 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * This file contains code for the NickServ FENFORCE function.
 */

#include "atheme-compat.h"

static void
ns_cmd_fenforce(sourceinfo_t *si, int parc, char *parv[])
{
	char *setting;
	myuser_t *mu;

	if (parc < 2)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "FENFORCE");
		command_fail(si, fault_needmoreparams, _("Syntax: FENFORCE <account> ON|OFF"));
		return;
	}

	mu = myuser_find_ext(parv[0]);
	if (!mu)
	{
		command_fail(si, fault_nosuch_target, _("\2%s\2 is not registered."), parv[0]);
		return;
	}
	setting = parv[1];

	if (strcasecmp(setting, "ON") == 0)
	{
		if (metadata_find(mu, "private:doenforce"))
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is already set for account \2%s\2."), "ENFORCE", entity(mu)->name);
		}
		else
		{
			wallops("%s enabled ENFORCE on the account \2%s\2.", get_oper_name(si), entity(mu)->name);
			logcommand(si, CMDLOG_ADMIN, "FENFORCE:ON: \2%s\2", entity(mu)->name);
			metadata_add(mu, "private:doenforce", "1");
			command_success_nodata(si, _("The \2%s\2 flag has been set for account \2%s\2."), "ENFORCE", entity(mu)->name);
		}
	}
	else if (strcasecmp(setting, "OFF") == 0)
	{
		if (metadata_find(mu, "private:doenforce"))
		{
			wallops("%s disabled ENFORCE on the account \2%s\2.", get_oper_name(si), entity(mu)->name);
			logcommand(si, CMDLOG_ADMIN, "FENFORCE:OFF: \2%s\2", entity(mu)->name);
			metadata_delete(mu, "private:doenforce");
			command_success_nodata(si, _("The \2%s\2 flag has been removed for account \2%s\2."), "ENFORCE", entity(mu)->name);
		}
		else
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is not set for account \2%s\2."), "ENFORCE", entity(mu)->name);
		}
	}
	else
	{
		command_fail(si, fault_badparams, STR_INVALID_PARAMS, "FENFORCE");
	}
}

static command_t ns_fenforce = {
	.name           = "FENFORCE",
	.desc           = N_("Enables or disables protection of another user's nicknames."),
	.access         = PRIV_USER_ADMIN,
	.maxparc        = 2,
	.cmd            = &ns_cmd_fenforce,
	.help           = { .path = "contrib/fenforce" },
};

static void
mod_init(module_t *const restrict m)
{
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/enforce");

	service_named_bind_command("nickserv", &ns_fenforce);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	service_named_unbind_command("nickserv", &ns_fenforce);
}

SIMPLE_DECLARE_MODULE_V1("contrib/ns_fenforce", MODULE_UNLOAD_CAPABILITY_OK)
