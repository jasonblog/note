/*
  Copyright (C) 1999-2002
  Andreas Gruenbacher, <a.gruenbacher@bestbits.at>
  shiwulo, <shiwulo@gmail.com>
  usage:./list_acl_simple ./list_acl.c
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
    acl_t acl; acl_type_t type;
    acl_entry_t entry; acl_tag_t tag;
    uid_t *uidp; gid_t *gidp;
    acl_permset_t permset;
    char *name;
    int entryId, permVal, opt, ret;

    type = ACL_TYPE_ACCESS;
    if ((opt = getopt(argc, argv, "def")) != -1) {
        type = ACL_TYPE_DEFAULT;
    }
    acl = acl_get_file(argv[optind], type);
    /* Walk through each entry in this ACL */
    for (entryId = ACL_FIRST_ENTRY; ; entryId = ACL_NEXT_ENTRY) {
        if (acl_get_entry(acl, entryId, &entry) != 1)
            break; /* error or no more entries */

        /* Retrieve and display tag type */
        acl_get_tag_type(entry, &tag);
        printf("%-12s", (tag == ACL_USER_OBJ) ?  "owner     " :
                        (tag == ACL_USER) ?      "user      " :
                        (tag == ACL_GROUP_OBJ) ? "file grp  " :
                        (tag == ACL_GROUP) ?     "group     " :
                        (tag == ACL_MASK) ?      "mask      " :
                        (tag == ACL_OTHER) ?     "other     " : "???");

        /* Retrieve and display optional tag qualifier */
        if (tag == ACL_USER) {
            uidp = (uid_t*) acl_get_qualifier(entry);
            printf("%-8s\t", getpwuid(*uidp)->pw_name);
        } else if (tag == ACL_GROUP) {
            gidp = (gid_t*) acl_get_qualifier(entry);
            printf("%-8s\t", getgrgid(*gidp)->gr_name);
        } else printf("\t\t");

        /* Retrieve and display permissions */
        acl_get_permset(entry, &permset);
        permVal = acl_get_perm(permset, ACL_READ);
        printf("%c", (permVal == 1) ? 'r' : '-');
        permVal = acl_get_perm(permset, ACL_WRITE);
        printf("%c", (permVal == 1) ? 'w' : '-');
        permVal = acl_get_perm(permset, ACL_EXECUTE);
        printf("%c", (permVal == 1) ? 'x' : '-');

        printf("\n");
    }
    acl_free(acl);
}
