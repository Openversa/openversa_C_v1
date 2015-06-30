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

#ifndef __OV_API__
#define __OV_API__

#define OV_C_API_VERSION_ID "1.1"

typedef void ov_handle_t;
typedef void ov_msg_handle_t;

typedef enum {
	OV_ERR_OK,
} OV_ERR;

/* Init */
ov_handle_t *ov_init(const char *app_id, const char *app_secret, const char *user_name, const char *url);
void ov_msg_cleanup(ov_msg_handle_t*);

/* Shutdown */
void ov_shutdown(ov_handle_t*);

/* Create group */
ov_msg_handle_t *ov_create_group(ov_handle_t*, char *group_name);

/* Delete group */
ov_msg_handle_t *ov_delete_group(ov_handle_t*, char *group_name);

/* Get group details */
typedef struct ov_get_group_details {
    int id;
    char *name;
    char *description;
    char *user_defined;
    int sent_msgs;
    int received_msgs;
    int member_id;
} ov_get_group_details_t;

ov_msg_handle_t *ov_get_group_details(ov_handle_t*, char *group_name, ov_get_group_details_t*);

/* Set group location */
typedef struct ov_group_location_param {
	char *group_name;
	char *latitude;
	char *longitude;
} ov_group_location_param_t;

ov_msg_handle_t *ov_set_group_location(ov_handle_t *handle, ov_group_location_param_t*);

/* Get [shared] group list */
typedef struct ov_group_status {
	char *date_created;
	char *name;
	int id;
	int avatar;
	int scheduled_count;
	int chat_count;
} ov_group_list_status_t;

typedef struct ov_group_list_param {
	int entry_count;
	ov_group_list_status_t *entries;
	int do_badges;
} ov_group_list_param_t;

ov_msg_handle_t *ov_get_group_list(ov_handle_t*, ov_group_list_param_t*);
ov_msg_handle_t *ov_get_shared_group_list(ov_handle_t*, ov_group_list_param_t*);

/* Delete group list */
ov_msg_handle_t *ov_delete_group_list(ov_handle_t*, int count, int *entries);

/* Create member */
typedef enum  {
	OV_MEMBER_TYPE_FACEBOOK = 1,
	OV_MEMBER_TYPE_EMAIL,
	OV_MEMBER_TYPE_SMS,
	OV_MEMBER_TYPE_TWITTER,
	OV_MEMBER_TYPE_MAX              // Last entry
} OV_MEMBER_TYPE;

typedef struct oauth {
	char *token;                // contact preference type social OAth token
	char *secret;               // contact preference type social OAth secret
} oauth_t;

typedef struct ov_member_param {
	char *group_name;
	char *first;                    // first name
	char *last;                     // last name
	char *phone;                    // US phone number no special characters or white space (if type SMS)
	char *email;                    // email of the person (only if the type is email)
	OV_MEMBER_TYPE type;            // contact preference type
	oauth_t *oauth;                 // contact preference type social OAth info
	char *device;                   // device ID of the mobile phone or device
} ov_member_param_t;

ov_msg_handle_t *ov_create_member(ov_handle_t*, ov_member_param_t*);

/* Delete member */
typedef enum  {
	OV_MEMBER_DELETE_FROM_THIS_GROUP,
	OV_MEMBER_DELETE_FROM_ALL_GROUPS,
} OV_MEMBER_DELETE_MODE;

ov_msg_handle_t *ov_delete_member(ov_handle_t*, char *group, int member_id);

/* Delete member list -- TODO */

/* Get members */
typedef struct ov_member_status {
	char *name;
	int id;
	char *phone_mobile;
	char *email;
	char *first_name;
	char *last_name;
	OV_MEMBER_TYPE contact_means;
} ov_member_status_t;

typedef struct ov_member_list {
	int entry_count;
	ov_member_status_t *entries;
	int group_id;
} ov_member_list_t;

ov_msg_handle_t *ov_get_member_list(ov_handle_t*, char *group_name, ov_member_list_t*);

/* Get member details */
ov_msg_handle_t *ov_get_member_details(ov_handle_t*, char *group_name, int id, ov_member_status_t*);

/* Set member details */
ov_msg_handle_t *ov_set_member_details(ov_handle_t*, char *group_name, int id, ov_member_param_t*);

/* Set member mobile number  -- TODO */

/* Set member contact type  -- TODO */

/* Get members timezone  -- TODO */

/* Create/Send message */
typedef struct ov_create_message {
	char *group;           // group name, limit 20 non-special char.
	int notify_member;     // flag to send the message to the members
	char *schedule;        // (optional) time if this message is to be sent on a schedule “2014-05-14 05:16:28”
	char *file_id;         // (optional) file ID of the file that was uploaded prior to OpenVersa
	int group_id;          // (optional) group ID
	int reply_id;          // (optional) ID of the message if this is a reply to a particular incoming message
	int direct_id;         // (optional) ID of the member you want to send a direct message to (not the entire group)
} ov_create_message_t;

ov_msg_handle_t *ov_create_message(ov_handle_t*, ov_create_message_t*, char *message);

/* Delete message */
ov_msg_handle_t *ov_delete_message(ov_handle_t*, int message_id);

/* Get messages [by time] */
typedef struct ov_get_message_req {
	char *group;                 // is your group name, limit 20 non-special char.
	int shared_group_id;         // (optional) is the group ID if shared.
	int group_id;                // (optional) is the group ID.
	time_t time;                 // time from which messages will be received (0 = all messages)
} ov_get_message_req_t;

typedef struct ov_get_message {
	char *message;               // the message in text
	int id;                    // message ID
	int member_id;             // the member ID of originator of the message
	char *added;                 // UTC time code of the create date
	char *user;                  // email of registered user of message creator
	int  attach_number;         // file ID number if attached
	char *attach_desc;           // name of the file if attached
	char *attach_storage;        // hash location of file in storage if attached to message
	char *first_name;            // the first name of the message creator
	char *last_name;             // the last name of the message creator
	OV_MEMBER_TYPE contact_means; // current type of the message creator
} ov_get_message_t;

typedef struct ov_get_message_list {
	int entry_count;
	ov_get_message_t *messages;
	int shared_group_id;         // (optional) is the group ID if shared.
	int group_id;                // (optional) is the group ID.
	time_t time;                 // time from which messages will be received (0 = all messages)
} ov_get_message_list_t;

ov_msg_handle_t *ov_get_messages(ov_handle_t*, char *group, ov_get_message_list_t*);

/* Get file list */
typedef struct ov_file_status {
	char *date_created;
	char *name;
	char *extension;
	int id;
	char *storage_name;
	int size;
	int member_id;
} ov_file_list_status_t;

typedef struct ov_file_list_param {
	int entry_count;
	ov_file_list_status_t *entries;
} ov_file_list_param_t;

ov_msg_handle_t *ov_get_file_list(ov_handle_t*, char *group, ov_file_list_param_t*);

/* Put file */

typedef struct ov_put_file_param {
	int notify_member;     // flag to notify the members
	char *name;            // filename (incl. extension)
	char *title;            // filename (incl. extension)
} ov_put_file_param_t;

ov_msg_handle_t *ov_put_file(ov_handle_t *handle, char *group, ov_put_file_param_t*);

/* Get file */

typedef struct ov_get_file_param {
	char *name;            // filename (incl. extension)
	char *local_path;      // local filepath (incl. extension)
	char *url;             // returned URL for downloading )
} ov_get_file_param_t;

ov_msg_handle_t *ov_get_file(ov_handle_t *handle, char *group, ov_get_file_param_t*);

/* Delete file */

typedef struct ov_delete_file_param {
	int notify_member;     // flag to notify the members
	char *name;            // filename (incl. extension)
	char *title;            // filename (incl. extension)
} ov_delete_file_param_t;

ov_msg_handle_t *ov_delete_file(ov_handle_t *handle, char *group, ov_delete_file_param_t*);

#endif // __OV_API__
