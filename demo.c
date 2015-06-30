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
#include <string.h>

#include "ov_api.h"

/* Set your OpenVersa Parameters here and remove this line! */
//#error "Set your OpenVersa Parameters"

#if 1
#define OPENVERSA_APP_ID        "INSERT YOUR APP KEY HERE"
#define OPENVERSA_APP_SECRET    "INSERT YOUR SECRET KEY HERE"
#define OPENVERSA_URL           "https://api.openversa.com/v2/"INSERT YOUR COMPANY NAME"/index.php"
#define OPENVERSA_USER_NAME     "INSERT YOUR USER NAME"
#else
#define OPENVERSA_APP_ID        "my_openversa_app_id"
#define OPENVERSA_APP_SECRET    "my_openversa_app_secret"
#define OPENVERSA_URL           "my_openversa_url"
#define OPENVERSA_USER_NAME     "my_openversa_email_address"
#endif // 1

/*
 * Print out the groups
 */

static int demo_print_group_list(ov_handle_t *ov_handle, int do_badges)
{
	ov_group_list_param_t gp = {
		.entry_count = -1,
		.do_badges = do_badges
	};
	ov_msg_handle_t *msg_handle = ov_get_group_list(ov_handle, &gp);

	if (msg_handle) {
		ov_group_list_status_t *ge;
		int i;
		printf("Received %d groups\n", gp.entry_count);
		printf("+----------------------+-------------------+------+------+\n");
		printf("|Group Name            |Date Created       |ID    |Chat #|\n");
		printf("+----------------------+-------------------+------+------+\n");
		for (i = 0, ge = gp.entries; i < gp.entry_count; i++, ge++) {
			printf("|%-22s|%19s|%6d|%6d|\n", ge->name, ge->date_created, ge->id, ge->chat_count);
		}
		printf("+----------------------+-------------------+------+------+\n");
		ov_msg_cleanup(msg_handle);
	}
	else {
		printf("OpenVersa group get failed\n");
	}
	return msg_handle ? 0 : 1;
}

/*
 * Print out the members in a group
 */

static int demo_print_member_list(ov_handle_t *ov_handle, char *group, int group_id)
{
	ov_member_list_t mp = {
		.entry_count = -1,
		.group_id = group_id
	};
	int i;
	ov_msg_handle_t *msg_handle = ov_get_member_list(ov_handle, group, &mp);

	if (msg_handle) {
		ov_member_status_t *me;
		printf("+--------------------------------------------------------------------------+\n");
		printf("|     Members In Group %-40s            |\n", group);
		printf("+--------------------------------------+------+-+--------------------------+\n");
		printf("|Name                                  |ID    |C|Email                     |\n");
		printf("+--------------------------------------+------+-+--------------------------+\n");
		for (i = 0, me = mp.entries; i < mp.entry_count; i++, me++) {
			printf("|%-38s|%6d|%d|%-26s|\n", me->name, me->id, me->contact_means, me->email);
		}
		printf("+--------------------------------------+------+-+--------------------------+\n");
		ov_msg_cleanup(msg_handle);
	}
	else {
		printf("OpenVersa member get failed for group %s\n", group);
	}
	return msg_handle ? 0 : 1;
}

/*
 * Print out the messages sent to a group
 */

static int demo_print_messages(ov_handle_t *ov_handle, char *group, int group_id)
{
	ov_get_message_t *ms;
	ov_get_message_list_t mp = {
		.entry_count = -1,
		.group_id = group_id
	};
	int i;
	ov_msg_handle_t *msg_handle = ov_get_messages(ov_handle, group, &mp);

	if (msg_handle) {
		printf("+-----------------------------------------------------------------------------+\n");
		printf("|     Messages to Group %-40s              |\n", group);
		printf("+------+--------------------------+-------------------------------------------+\n");
		printf("|Msg ID|Added              |From  |Message                                    |\n");
		printf("+------+-------------------+------+-------------------------------------------+\n");
		for (i = 0, ms = mp.messages; i < mp.entry_count; i++, ms++) {
			printf("|%6d|%19s|%6d|%-43s|\n", ms->id, ms->added, ms->member_id, ms->message);
		}
		printf("+------+-------------------+------+-------------------------------------------+\n");
		ov_msg_cleanup(msg_handle);
	}
	else {
		printf("OpenVersa message get failed for group %s\n", group);
	}
	return msg_handle ? 0 : 1;
}

/*
 * Print out a group's file list
 */

static int demo_print_file_list(ov_handle_t *ov_handle, char *group)
{
    ov_file_list_param_t flp = {};
	int i;
    ov_msg_handle_t *msg_handle = ov_get_file_list(ov_handle, group, &flp);
    if (msg_handle) {
        ov_file_list_status_t *fs;
		printf("+-----------------------------------------------------------------------------+\n");
		printf("|     Files In to Group %-40s              |\n", group);
		printf("+------+--------------------------+-------------------------------------------+\n");
		printf("|FileID|Added              |From  |Name                                       |\n");
		printf("+------+-------------------+------+-------------------------------------------+\n");
		for (i = 0, fs = flp.entries; i < flp.entry_count; i++, fs++) {
			printf("|%6d|%19s|%6d|%-43s|\n", fs->id, fs->date_created, fs->member_id, fs->name);
		}
		printf("+------+-------------------+------+-------------------------------------------+\n");
		ov_msg_cleanup(msg_handle);
    }
	else {
		printf("OpenVersa file list get failed for group %s\n", group);
	}
    return msg_handle ? 0 : 1;
}

static const char *test_step_desc[] = {
	" 1: get group list",
	" 2: create new group 'OV_Test_ABCDEF'",
	" 3: get group list again, has new group been added?",
	" 4: delete group 'OV_Test_ABCDEF'",
	" 5: get group list again, group should have gone",
	" 6: create new group 'OV_Test_ABCDEF' again",
	" 7: Show all members of all groups",
	" 8: Add a member to 'OV_Test_ABCDEF'",
	" 9: Show all members of 'OV_Test_ABCDEF'; 'Sally Jones' should have been added",
	"10: Delete 'Sally Jones' from group 'OV_Test_ABCDEF'",
	"11: Show all members of 'OV_Test_ABCDEF'; 'Sally Jones' should have been removed",
	"12: Send 3 messages to 'OV_Test_ABCDEF'",
	"13: Get messages sent to 'OV_Test_ABCDEF' - Should see 'OpenVersa API Test Message'",
	"14: Remove all messages from the group 'SprinkerScrooge_ABCDEF'",
	"15: Get messages sent to 'OV_Test_ABCDEF'. Should be empty",
	"16: Get file list for group 'SprinkerScrooge_ABCDEF'. Should be empty",
	"17: upload file 'abc.txt' group 'SprinkerScrooge_ABCDEF'",
	"18: get file list for group 'SprinkerScrooge_ABCDEF', is 'abc.txt' there?",
	"19: download file 'abc.txt' group 'SprinkerScrooge_ABCDEF'",
	"20: delete group 'SprinkerScrooge_ABCDEF'",
	"21: get group list again, group should have gone",
	"22: The End!",
};

int main(int argc, char *argv[])
{
	ov_handle_t *ov_handle;
	ov_msg_handle_t *msg_handle, *msg_handle2;
	int rc = 0, test_no = 1, i;

	if (!((ov_handle = ov_init(OPENVERSA_APP_ID, OPENVERSA_APP_SECRET, OPENVERSA_USER_NAME, OPENVERSA_URL)))) {
		printf("OpenVersa initialization failed\n");
		return 0;
	}

	while (rc == 0) {
		printf("======================== TEST STEP %s =====================\n", test_step_desc[test_no - 1]);

        if (test_no > 14) {
                /* Uncomment to pause for each step */
                printf("press a key to continue...");
                getchar();
                printf("\n");
        }
		switch (test_no++) {
		case 1: /* get group list */
			rc = demo_print_group_list(ov_handle, 0);
			break;
		case 2: /* create new group "SprinkerScrooge_ABCDEF" */
			if ((msg_handle = ov_create_group(ov_handle, "SprinkerScrooge_ABCDEF"))) {
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
			break;
		case 3: /* get group list again, has new group been added? */
			rc = demo_print_group_list(ov_handle, 0);
			break;
		case 4: /* delete group "SprinkerScrooge_ABCDEF" */
			if ((msg_handle = ov_delete_group(ov_handle, "SprinkerScrooge_ABCDEF"))) {
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
			break;
		case 5: /* get group list again, group should have gone */
			rc = demo_print_group_list(ov_handle, 0);
			break;
		case 6: /* create new group "SprinkerScrooge_ABCDEF" again */
			if ((msg_handle = ov_create_group(ov_handle, "SprinkerScrooge_ABCDEF"))) {
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
			break;
		case 7: { /* Show all members of all groups */
			ov_group_list_param_t gp = {};
			if ((msg_handle = ov_get_group_list(ov_handle, &gp))) {
				ov_group_list_status_t *ge;
				for (i = 0, ge = gp.entries; (i < gp.entry_count) && (rc == 0); i++, ge++) {
					rc = demo_print_member_list(ov_handle, ge->name, ge->id);
				}
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
			break;
		}
		case 8: { /* Add a member to "SprinkerScrooge_ABCDEF" */
			ov_member_param_t mp = {
				.group_name = "SprinkerScrooge_ABCDEF",
				.first = "Sally",                    // the first name
				.last = "Jones",                     // the last name
				.phone = "3105551212",                    // the US phone number no special characters or white space (if type SMS)
				.email = "aunt.sally@aol.com",           // the email of the person (only if the type is email)
				.type = OV_MEMBER_TYPE_EMAIL            // the contact preference type
			};
			if ((msg_handle = ov_create_member(ov_handle, &mp))) {
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
			break;
		}
		case 9: /* Show all members of "SprinkerScrooge_ABCDEF"; "Sally Jones" should have been added */
			rc = demo_print_member_list(ov_handle, "SprinkerScrooge_ABCDEF", 0);
			break;
		case 10: { /* Delete "Sally Jones" from group "SprinkerScrooge_ABCDEF" */
			/* Scan the member list to find the member ID for Sally Jones */
			ov_member_list_t mp = {
				.entry_count = -1,
				.group_id = 0
			};
			if ((msg_handle = ov_get_member_list(ov_handle, "SprinkerScrooge_ABCDEF", &mp))) {
				ov_member_status_t *me;
				for (i = 0, me = mp.entries; i < mp.entry_count; i++, me++) {
					if (strcmp(me->first_name, "Sally") || strcmp(me->last_name, "Jones")) {
						/* this is not Sally, skip */
						continue;
					}
					/* found it - delete it */
					if ((msg_handle2 = ov_delete_member(ov_handle, "SprinkerScrooge_ABCDEF", me->id))) {
						ov_msg_cleanup(msg_handle2);
					}
					rc = msg_handle2 ? 0 : 1;
					break;
				}
				if (i == mp.entry_count) {
					/* member not found, error */
					rc = 1;
				}
				ov_msg_cleanup(msg_handle);
			}
			break;
		}
		case 11: /* Show all members of "SprinkerScrooge_ABCDEF"; "Sally Jones" should have been removed */
			rc = demo_print_member_list(ov_handle, "SprinkerScrooge_ABCDEF", 0);
			break;
		case 12: { /* Send 3 messages to "SprinkerScrooge_ABCDEF" */
			ov_create_message_t cm = {
				.group = "SprinkerScrooge_ABCDEF",
				.notify_member = 1,
			};
			char msg_buf[64];
			for (i = 0; (i < 3) && (rc == 0); i++) {
				sprintf(msg_buf, "OpenVersa API Test Message %d", i + 1);
				if ((msg_handle = ov_create_message(ov_handle, &cm, msg_buf))) {
					ov_msg_cleanup(msg_handle);
				}
				rc = msg_handle ? 0 : 1;
			}
			break;
		}
		case 13: /* Retrieve the messages that were sent to "SprinkerScrooge_ABCDEF".
                  * Should see "OpenVersa API Test Message" in the list
                  */
			rc = demo_print_messages(ov_handle, "SprinkerScrooge_ABCDEF", 0);
			break;
		case 14: { /* Remove all messages from the group "SprinkerScrooge_ABCDEF" */
			ov_get_message_t *ms;
			ov_get_message_list_t mp = {};
			if ((msg_handle = ov_get_messages(ov_handle, "SprinkerScrooge_ABCDEF", &mp))) {
				for (i = 0, ms = mp.messages; rc == 0 && (i < mp.entry_count); i++, ms++) {
					if ((msg_handle2 = ov_delete_message(ov_handle, ms->id))) {
						ov_msg_cleanup(msg_handle2);
					}
					rc = msg_handle2 ? 0 : 1;
				}
			}
			rc |= msg_handle ? 0 : 1;
			break;
		}
		case 15: /* Retrieve the messages that were sent to "SprinkerScrooge_ABCDEF".
                  * Should be empty
                  */
			rc = demo_print_messages(ov_handle, "SprinkerScrooge_ABCDEF", 0);
			break;
		case 16: /* get file list for group "SprinkerScrooge_ABCDEF", should be empty */
            rc = demo_print_file_list(ov_handle, "SprinkerScrooge_ABCDEF");
			break;
		case 17: { /* upload file "abc.txt" group "SprinkerScrooge_ABCDEF" */
            FILE *fp = fopen("abc.txt", "w");
            if (fp)  {
                ov_put_file_param_t pfp = {
                    .notify_member = 0,
                    .name = "abc.txt",
                    .title = "abc.txt"
                };
                fwrite("TEST\n", 1, sizeof("TEST\n"), fp);
                fclose(fp);

                if ((msg_handle = ov_put_file(ov_handle, "SprinkerScrooge_ABCDEF", &pfp))) {
                    ov_msg_cleanup(msg_handle);
                }
            }
			rc = msg_handle ? 0 : 1;
			break;
        }
		case 18: /* get file list for group "SprinkerScrooge_ABCDEF", is "abc.txt" there? */
            rc = demo_print_file_list(ov_handle, "SprinkerScrooge_ABCDEF");
			break;
		case 19: { /* download file "abc.txt" group "SprinkerScrooge_ABCDEF" */
#if 1 // NOT_YET
            ov_get_file_param_t gfp = {
                .name = "abc.txt",
                .local_path = "def.txt"
            };
			if ((msg_handle = ov_get_file(ov_handle, "SprinkerScrooge_ABCDEF", &gfp))) {
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
#else
            rc = 0;
#endif // NOT_YET
			break;
        }
		case 20: /* delete group "SprinkerScrooge_ABCDEF" */
			if ((msg_handle = ov_delete_group(ov_handle, "SprinkerScrooge_ABCDEF"))) {
				ov_msg_cleanup(msg_handle);
			}
			rc = msg_handle ? 0 : 1;
			break;
		case 21: /* get group list again, group should have gone */
			rc = demo_print_group_list(ov_handle, 0);
			break;
		default:
			/* we're done */
			rc = 1;
			break;
		}
	}
	ov_shutdown(ov_handle);
	return 0;
}
