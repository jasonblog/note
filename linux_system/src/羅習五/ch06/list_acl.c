/*
  Copyright (C) 1999-2002
  Andreas Gruenbacher, <a.gruenbacher@bestbits.at>
  shiwulo, <shiwulo@gmail.com>
  usage: ./list_acl ./list_acl.c
 */ 
#include <acl/libacl.h>
#include <stdio.h>
#include <sys/acl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <attr/xattr.h>


int main(int argc, char *argv[])
{
    acl_t acl;
    acl_type_t type;
    acl_entry_t entry;
    acl_tag_t tag;
    uid_t *uidp;
    gid_t *gidp;
    acl_permset_t permset;
    char *name;
    int entryId, permVal, opt;
    char buf[132];
    int ret;

    type = ACL_TYPE_ACCESS;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
        case 'd': type = ACL_TYPE_DEFAULT;      break;
        }
    }
    acl = acl_get_file(argv[optind], type);
    /* Walk through each entry in this ACL */
    for (entryId = ACL_FIRST_ENTRY; ; entryId = ACL_NEXT_ENTRY) {

        if (acl_get_entry(acl, entryId, &entry) != 1)
            break;                      /* Exit on error or no more entries */

        /* Retrieve and display tag type */
        assert(acl_get_tag_type(entry, &tag) != -1);
        printf("%-12s", (tag == ACL_USER_OBJ) ?  "user_obj" :
                        (tag == ACL_USER) ?      "user" :
                        (tag == ACL_GROUP_OBJ) ? "group_obj" :
                        (tag == ACL_GROUP) ?     "group" :
                        (tag == ACL_MASK) ?      "mask" :
                        (tag == ACL_OTHER) ?     "other" : "???");

        /* Retrieve and display optional tag qualifier */
        if (tag == ACL_USER) {
            uidp = (uid_t*) acl_get_qualifier(entry);
            assert(uidp!=NULL);
            name = getpwuid(*uidp)->pw_name;
            if (name == NULL)
                printf("%-8d ", *uidp);
            else
                printf("%-8s ", name);
            assert(acl_free(uidp) != -1);
        } else if (tag == ACL_GROUP) {
            gidp = (gid_t*) acl_get_qualifier(entry);
            assert(gidp != NULL);
            name = getgrgid(*gidp)->gr_name;
            if (name == NULL)
                printf("%-8d ", *gidp);
            else
                printf("%-8s ", name);
            assert(acl_free(gidp) != -1);
        } else {
            printf("         ");
        }

        /* Retrieve and display permissions */
        acl_get_permset(entry, &permset);
        permVal = acl_get_perm(permset, ACL_READ);
        assert(permVal != -1);
        printf("%c", (permVal == 1) ? 'r' : '-');
        permVal = acl_get_perm(permset, ACL_WRITE);
        assert(permVal != -1);
        printf("%c", (permVal == 1) ? 'w' : '-');
        permVal = acl_get_perm(permset, ACL_EXECUTE);
        assert(permVal != -1);
        printf("%c", (permVal == 1) ? 'x' : '-');

        printf("\n");
    }
    assert(acl_free(acl) != -1);
}