/*

    COPYRIGHT AND PERMISSION NOTICE
    Copyright (c) 2015 miVersa Inc
    All rights reserved.
    Permission to use, copy, modify, and distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall
    not be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "ov_jsmn.h"
#include "ov_api.h"
#include "ov_curl.h"

void ov_msg_cleanup(ov_msg_handle_t *ov_msg_handle)
{
	return ov_curl_msg_cleanup(ov_msg_handle);
}

/***********************************************************************************
 *
 * Get [shared] group list
 */

static void ov_group_list_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	jsmntok_t *t;

	if ((t = jsmn_tok(tokens, data, "{result"))) {
		int count = t->size;
		ov_group_list_param_t *glt = (ov_group_list_param_t*) user_data;
		ov_group_list_status_t *gs = glt->entries = calloc(count, sizeof(ov_group_list_status_t));
		glt->entry_count = count;
		t = jsmn_tok(t, data, "[");
		while (t && count--) {
			gs->name         = jsmn_get_string(t, data, "Group_Name");
			gs->date_created = jsmn_get_string(t, data, "Date_Created");
			gs->id           = jsmn_get_int(t, data, "Group_ID");
			gs->chat_count   = jsmn_get_int(t, data, "Chat_Count");
			gs->scheduled_count = jsmn_get_int(t, data, "Scheduled_Count");
			gs->avatar       = jsmn_get_int(t, data, "Avatar");
			gs++;
			t = jsmn_skip(t);
		}
	}
}

static void ov_group_list_json_cleanup(void *data)
{
	ov_group_list_param_t *glt = data;

	if (glt->entries) {
		ov_group_list_status_t *gs = glt->entries;
		while (glt->entry_count--) {
			free(gs->name);
			free(gs->date_created);
			gs++;
		}
		free(glt->entries);
		glt->entries = NULL;
	}
	glt->entry_count = 0;
}

static ov_msg_handle_t *ov_get_group_list_common(ov_handle_t *handle, ov_group_list_param_t *glp, char *command)
{
	ov_msg_handle_t *msg_handle = ov_curl_msg_init(handle, command, ov_group_list_json_cleanup, glp);

	if (msg_handle) {
		ov_curl_set_int(msg_handle, "doBadges", glp->do_badges);
		if (ov_curl_json(msg_handle, ov_group_list_json_parse, glp)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

ov_msg_handle_t *ov_get_group_list(ov_handle_t *handle, ov_group_list_param_t *glp)
{
	return ov_get_group_list_common(handle, glp, "getGroupList");
}

ov_msg_handle_t *ov_get_shared_group_list(ov_handle_t *handle, ov_group_list_param_t *glp)
{
	return ov_get_group_list_common(handle, glp, "getOtherGroups");
}

/*
 ***************************** Get member list *****************************
 */

static void ov_member_list_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	jsmntok_t *t;

	if ((t = jsmn_tok(tokens, data, "{result"))) {
		int count = t->size;
		ov_member_list_t *glp = (ov_member_list_t*) user_data;
		ov_member_status_t *gs = glp->entries = calloc(count, sizeof(ov_member_status_t));
		glp->entry_count = count;
		t = jsmn_tok(t, data, "[");
		while (t && count--) {
			gs->name         = jsmn_get_string(t, data, "Member_Name");
			gs->id           = jsmn_get_int(t, data, "Member_Name_ID");
			gs->phone_mobile = jsmn_get_string(t, data, "Phone_Mobile");
			gs->email        = jsmn_get_string(t, data, "Email");
			gs->first_name   = jsmn_get_string(t, data, "First_Name");
			gs->last_name    = jsmn_get_string(t, data, "Last_Name");
			gs->contact_means = jsmn_get_int(t, data, "Contact_Means");
			gs++;
			t = jsmn_skip(t);
		}
	}
}

void ov_member_list_json_cleanup(void *data)
{
	ov_member_list_t *mlp = data;
	if (mlp->entries) {
		ov_member_status_t *me = mlp->entries;
		while (mlp->entry_count--) {
			free(me->name);
			free(me->phone_mobile);
			free(me->email);
			free(me->first_name);
			free(me->last_name);
			me++;
		}
		free(mlp->entries);
		mlp->entries = NULL;
	}
	mlp->entry_count = 0;
}

ov_msg_handle_t *ov_get_member_list(ov_handle_t *handle, char *group_name, ov_member_list_t *mlp)
{
	ov_msg_handle_t *msg_handle = ov_curl_msg_init(handle, "getMembers", ov_member_list_json_cleanup, mlp);

	if (msg_handle) {
		if (group_name) {
			ov_curl_set_string(msg_handle, "group", group_name);
		}
		if (mlp->group_id) {
			ov_curl_set_int(msg_handle, "groupID", mlp->group_id);
		}
		if (ov_curl_json(msg_handle, ov_member_list_json_parse, mlp)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Create Group *****************************
 */
ov_msg_handle_t *ov_create_group(ov_handle_t *handle, char *group_name)
{
	ov_msg_handle_t *msg_handle = ov_curl_msg_init(handle, "createGroup", NULL, NULL);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", group_name);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Delete Group *****************************
 */

ov_msg_handle_t *ov_delete_group(ov_handle_t *handle, char *group_name)
{
	ov_msg_handle_t *msg_handle = ov_curl_msg_init(handle, "deleteGroup", NULL, NULL);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", group_name);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Get Group Details *****************************
 */

static void ov_get_group_details_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	jsmntok_t *t;

	if ((t = jsmn_tok(tokens, data, "{result"))) {
		ov_get_group_details_t *ggd = (ov_get_group_details_t*)user_data;
        ggd->id             = jsmn_get_int   (t, data, "Group_ID");
        ggd->name           = jsmn_get_string(t, data, "Group_Name");
        ggd->description    = jsmn_get_string(t, data, "Description");
        ggd->user_defined   = jsmn_get_string(t, data, "User_Defined");
        ggd->sent_msgs      = jsmn_get_int   (t, data, "Sent_Messages");
        ggd->received_msgs  = jsmn_get_int   (t, data, "Received_Messages");
        ggd->member_id      = jsmn_get_int   (t, data, "Member_ID");           // Does this make sense?
	}
}

static void ov_get_group_details_json_cleanup(void *data)
{
	ov_get_group_details_t *gdd = data;

    free(gdd->name);
    free(gdd->description);
    free(gdd->user_defined);
}

ov_msg_handle_t *ov_get_group_details(ov_handle_t *handle, char *group_name, ov_get_group_details_t *ggd)
{
	ov_msg_handle_t *msg_handle = ov_curl_msg_init(handle, "getGroupDetails", ov_get_group_details_json_cleanup, ggd);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", group_name);
		if (ov_curl_json(msg_handle, ov_get_group_details_json_parse, ggd)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Set Group Location *****************************
 */

ov_msg_handle_t *ov_set_group_location(ov_handle_t *handle, ov_group_location_param_t *glp)
{
	void *msg_handle = ov_curl_msg_init(handle, "setGroupLocation", NULL, NULL);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", glp->group_name);
		ov_curl_set_string(msg_handle, "longitude", glp->longitude);
		ov_curl_set_string(msg_handle, "latitude", glp->latitude);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Create Member *****************************
 */

ov_msg_handle_t *ov_create_member(ov_handle_t *handle, ov_member_param_t *mp)
{
	void *msg_handle = ov_curl_msg_init(handle, "createMember", NULL, NULL);

	if (msg_handle) {
		char first_period_last[80];
		ov_curl_set_string(msg_handle, "group", mp->group_name);
		snprintf(first_period_last, 79, "%s.%s", mp->first? mp->first : "first", mp->last? mp->last : "last");
		ov_curl_set_string(msg_handle, "firstPeriodLast", first_period_last);
		if (mp->first) {
			ov_curl_set_string(msg_handle, "first", mp->first);
		}
		if (mp->last) {
			ov_curl_set_string(msg_handle, "last", mp->last);
		}
		if (mp->phone) {
			ov_curl_set_string(msg_handle, "phone", mp->phone);
		}
		if (mp->email) {
			ov_curl_set_string(msg_handle, "email", mp->email);
		}
		ov_curl_set_int(msg_handle, "type", mp->type);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Delete Member *****************************
 */

ov_msg_handle_t *ov_delete_member(ov_handle_t *handle, char *group, int member_id)
{
	void *msg_handle = ov_curl_msg_init(handle, "removeMemberFromGroup", NULL, NULL);

	if (msg_handle) {
		if (group) {
			/* group specified, remove from just this group */
			ov_curl_set_string(msg_handle, "group", group);
			ov_curl_set_string(msg_handle, "memberID", "sprinklerscrooge@yahoo.com");
			ov_curl_set_int(msg_handle, "removeFlag", 0);
		}
		else {
			/* remove from all groups */
			ov_curl_set_int(msg_handle, "removeFlag", 1);
		}
		ov_curl_set_int(msg_handle, "memberRemoveID", member_id);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Create Message *****************************
 */
#if NOT_YET /* Wait for return codes to get formalized */
static void ov_create_message_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	ov_create_message_t *cm = (ov_create_message_t*) user_data;

	cm->response.code = jsmn_get_int(tokens, data, "code");
	cm->response.success = jsmn_get_string(tokens, data, "success");
}

static void ov_create_message_json_cleanup(ov_create_message_t *cm)
{
	free(cm->response.success);
	cm->response.success = NULL;
}

#endif // NOT_YET

ov_msg_handle_t *ov_create_message(ov_handle_t*handle, ov_create_message_t *cm, char *message)
{
	void *msg_handle = ov_curl_msg_init(handle, "createMessage", NULL, NULL);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", cm->group);
		if (cm->group_id) {
			ov_curl_set_int(msg_handle, "groupID", cm->group_id);
		}
		ov_curl_set_int(msg_handle, "notifyMembers", cm->notify_member);
		ov_curl_set_string(msg_handle, "message", message);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Delete Message *****************************
 */

ov_msg_handle_t *ov_delete_message(ov_handle_t *handle, int message_id)
{
	void *msg_handle = ov_curl_msg_init(handle, "deleteMessage", NULL, NULL);

	if (msg_handle) {
		ov_curl_set_int(msg_handle, "messageID", message_id);
		if (ov_curl_json(msg_handle, NULL, NULL)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}
/*
 ***************************** Get Messages *****************************
 */

static void ov_get_messages_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	jsmntok_t *t;

	if ((t = jsmn_tok(tokens, data, "{result"))) {
		int count = t->size;
		ov_get_message_list_t *gml = (ov_get_message_list_t*) user_data;
		ov_get_message_t *gm = gml->messages = calloc(count, sizeof(ov_get_message_t));
		gml->entry_count = count;
		t = jsmn_tok(t, data, "[");
		while (t && count--) {
			gm->message        = jsmn_get_string(t, data, "Message");
			gm->id             = jsmn_get_int(t, data, "Id");
			gm->member_id      = jsmn_get_int(t, data, "Member_ID");
			gm->added          = jsmn_get_string(t, data, "Added");
			gm->user           = jsmn_get_string(t, data, "User");
			gm->attach_number  = jsmn_get_int(t, data, "Attach_Number");
			gm->attach_storage = jsmn_get_string(t, data, "Attach_Storage");
			gm->attach_desc    = jsmn_get_string(t, data, "Attach_Desc");
			gm->first_name     = jsmn_get_string(t, data, "First_Name");
			gm->last_name      = jsmn_get_string(t, data, "Last_Name");
			gm->contact_means  = jsmn_get_int(t, data, "Contact_Means");
			gm++;
			t = jsmn_skip(t);
		}
	}
}

static void ov_get_messages_json_cleanup(void *data)
{
	ov_get_message_list_t *gml = data;
	if (gml->messages) {
		ov_get_message_t *gm = gml->messages;
		while (gml->entry_count--) {
			free(gm->message);
			free(gm->added);
			free(gm->user);
			free(gm->attach_storage);
			free(gm->attach_desc);
			free(gm->first_name);
			free(gm->last_name);
			gm++;
		}
		free(gml->messages);
		gml->messages = NULL;
	}
	gml->entry_count = 0;
}

ov_msg_handle_t *ov_get_messages(ov_handle_t *handle, char *group, ov_get_message_list_t *gml)
{
	void *msg_handle = ov_curl_msg_init(handle, "getMessages", ov_get_messages_json_cleanup, gml);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", group);
		if (ov_curl_json(msg_handle, ov_get_messages_json_parse, gml)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}

/*
 ***************************** Get File List *****************************
 */

ov_msg_handle_t *ov_get_file_list(ov_handle_t*, char *group, ov_file_list_param_t*);

static void ov_get_file_list_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	jsmntok_t *t;

	if ((t = jsmn_tok(tokens, data, "{result"))) {
		int count = t->size;
		ov_file_list_param_t *gfl = (ov_file_list_param_t*) user_data;
		ov_file_list_status_t *gf = gfl->entries = calloc(count, sizeof(ov_file_list_status_t));
		gfl->entry_count = count;
		t = jsmn_tok(t, data, "[");
		while (t && count--) {
			gf->id             = jsmn_get_int(t, data, "File_ID");
			gf->date_created   = jsmn_get_string(t, data, "Added_Date");
			gf->name           = jsmn_get_string(t, data, "File_Name");
			gf->extension      = jsmn_get_string(t, data, "Extension");
			gf->member_id      = jsmn_get_int(t, data, "Member_ID");
			gf->storage_name   = jsmn_get_string(t, data, "Storage_Name");
			gf++;
			t = jsmn_skip(t);
		}
	}
}

static void ov_get_file_list_json_cleanup(void *data)
{
	ov_file_list_param_t *gfl = data;
	if (gfl->entries) {
		ov_file_list_status_t *gf = gfl->entries;
		while (gfl->entry_count--) {
			free(gf->date_created);
			free(gf->name);
			free(gf->extension);
			free(gf->storage_name);
			gf++;
		}
		free(gfl->entries);
		gfl->entries = NULL;
	}
	gfl->entry_count = 0;
}

ov_msg_handle_t *ov_get_file_list(ov_handle_t *handle, char *group, ov_file_list_param_t *flp)
{
	void *msg_handle = ov_curl_msg_init(handle, "getFileList", ov_get_file_list_json_cleanup, flp);

	if (msg_handle) {
		ov_curl_set_string(msg_handle, "group", group);
		if (ov_curl_json(msg_handle, ov_get_file_list_json_parse, flp)) {
			/* error */
			ov_curl_msg_cleanup(msg_handle);
			msg_handle = NULL;
		}
	}
	return msg_handle;
}


/*
 ***************************** Put File *****************************
 */
#if 0
static void ov_put_file_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	jsmntok_t *t;

	if ((t = jsmn_tok(tokens, data, "{result"))) {
		int count = t->size;
		ov_file_list_param_t *gfl = (ov_file_list_param_t*) user_data;
		ov_file_list_status_t *gf = gfl->entries = calloc(count, sizeof(ov_file_list_status_t));
		gfl->entry_count = count;
		t = jsmn_tok(t, data, "[");
		while (t && count--) {
			gf->id             = jsmn_get_int(t, data, "File_ID");
			gf->date_created   = jsmn_get_string(t, data, "Added_Date");
			gf->name           = jsmn_get_string(t, data, "File_Name");
			gf->extension      = jsmn_get_string(t, data, "Extension");
			gf->member_id      = jsmn_get_int(t, data, "Member_ID");
			gf->storage_name   = jsmn_get_string(t, data, "Storage_Name");
			gf++;
			t = jsmn_skip(t);
		}
	}
}

static void ov_put_file_json_cleanup(void *data)
{
	ov_file_list_param_t *gfl = data;
	if (gfl->entries) {
		ov_file_list_status_t *gf = gfl->entries;
		while (gfl->entry_count--) {
			free(gf->date_created);
			free(gf->name);
			free(gf->extension);
			free(gf->storage_name);
			gf++;
		}
		free(gfl->entries);
		gfl->entries = NULL;
	}
	gfl->entry_count = 0;
}
#endif //0
ov_msg_handle_t *ov_put_file(ov_handle_t *handle, char *group, ov_put_file_param_t *pfp)
{
	void *msg_handle;
    struct stat file_info;

    /* get the file size of the local file */
    if (stat(pfp->name, &file_info)) {
        /* file error, bail */
        return NULL;
    }

    if ((msg_handle = ov_curl_msg_init(handle, "uploadFile", NULL, pfp))) {
        ov_curl_set_string(msg_handle, "group", group);
        ov_curl_set_int(msg_handle, "notifyMembers", pfp->notify_member);
        ov_curl_set_int(msg_handle, "filesize", file_info.st_size);
        ov_curl_set_string(msg_handle, "title", pfp->title);
        ov_curl_set_string(msg_handle, "name", pfp->name);
        ov_curl_set_filedata(msg_handle, "file", pfp->name);
        if (ov_curl_json(msg_handle, NULL, pfp)) {
            /* error */
            ov_curl_msg_cleanup(msg_handle);
            msg_handle = NULL;
        }
    }
	return msg_handle;
}

/**************************** Get file ****************************/
#if 0
typedef struct ov_get_file_int_param {
	ov_get_file_param_t *gfp;
	char *url;
} ov_get_file_int_param_t;
#endif // 0
static void ov_get_file_json_parse(void *ov_msg_handle, jsmntok_t *tokens, char *data, void *user_data)
{
	ov_get_file_param_t *gfp = (ov_get_file_param_t*) user_data;

	gfp->url = jsmn_get_string(tokens, data, "File");
}

static void ov_get_file_json_cleanup(void *data)
{
    ov_get_file_param_t *gfp = (ov_get_file_param_t*)data;

	if (gfp->url) free(gfp->url);
}

ov_msg_handle_t *ov_get_file(ov_handle_t *handle, char *group, ov_get_file_param_t *gfp)
{
	void *msg_handle;

    if ((msg_handle = ov_curl_msg_init(handle, "getFile", ov_get_file_json_cleanup, gfp))) {
        ov_curl_set_string(msg_handle, "group", group);
        ov_curl_set_string(msg_handle, "fileID", "0");
        ov_curl_set_string(msg_handle, "fileName", gfp->name);
        if ((ov_curl_json(msg_handle, ov_get_file_json_parse, &gfp) || gfp->url == NULL)) {
            /* error */
            ov_curl_msg_cleanup(msg_handle);
            msg_handle = NULL;
        }
        else {
            /* we've retrieved the file URL, now download the file */
            ov_curl_download(gfp->local_path, gfp->url);
        }
    }
	return msg_handle;
}

/* Delete file */
#if NOT_YET
ov_msg_handle_t *ov_delete_file(ov_handle_t *handle, char *group, ov_delete_file_param_t*)
{
}
#endif // NOT_YET
