
mkdir %1
mkdir %1\dll
copy ..\..\trunk\dokan\objchk_%1_x86\i386\dokan.dll %1\dll
copy ..\..\trunk\dokan\objchk_%1_x86\i386\dokan.lib %1\dll
copy ..\..\trunk\dokan\objchk_%1_x86\i386\dokan.pdb %1\dll
mkdir %1\dokanctrl
copy ..\..\trunk\dokan_control\objchk_%1_x86\i386\dokanctl.exe %1\dokanctrl\
copy ..\..\trunk\dokan_control\objchk_%1_x86\i386\dokanctl.pdb %1\dokanctrl\
mkdir %1\mirror
copy ..\..\trunk\dokan_mirror\objchk_%1_x86\i386\mirror.exe %1\mirror\
copy ..\..\trunk\dokan_mirror\objchk_%1_x86\i386\mirror.pdb %1\mirror\
mkdir %1\mounter
copy ..\..\trunk\dokan_mount\objchk_%1_x86\i386\mounter.exe %1\mounter\
copy ..\..\trunk\dokan_mount\objchk_%1_x86\i386\mounter.pdb %1\mounter\
mkdir %1\sys
copy ..\..\trunk\sys\objchk_%1_x86\i386\dokan.sys %1\sys\
copy ..\..\trunk\sys\objchk_%1_x86\i386\dokan.pdb %1\sys\


