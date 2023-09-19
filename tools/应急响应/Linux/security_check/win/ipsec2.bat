REM =================��ʼ================ 
netsh ipsec static add policy name=safedog 

REM ���2��������block��permit 
netsh ipsec static add filteraction name=Permit action=permit 
netsh ipsec static add filteraction name=Block action=block 

REM ���Ƚ�ֹ���з��� 
netsh ipsec static add filterlist name=AllAccess 
netsh ipsec static add filter filterlist=AllAccess srcaddr=Me dstaddr=Any 
netsh ipsec static add rule name=BlockAllAccess policy=safedog  filterlist=AllAccess filteraction=Block 

REM ����ĳЩIP�����Ʒ��� 
netsh ipsec static add filterlist name=UnLimitedIP 
netsh ipsec static add filter filterlist=UnLimitedIP srcaddr=61.128.128.67 dstaddr=Me 
netsh ipsec static add rule name=AllowUnLimitedIP policy=safedog  filterlist=UnLimitedIP filteraction=Permit 

REM ����ĳЩ�˿� 
netsh ipsec static add filterlist name=OpenSomePort 
netsh ipsec static add filter filterlist=OpenSomePort srcaddr=Any dstaddr=Me dstport=20 protocol=TCP 
netsh ipsec static add filter filterlist=OpenSomePort srcaddr=Any dstaddr=Me dstport=21 protocol=TCP 
netsh ipsec static add filter filterlist=OpenSomePort srcaddr=Any dstaddr=Me dstport=80 protocol=TCP 
netsh ipsec static add filter filterlist=OpenSomePort srcaddr=Any dstaddr=Me dstport=3389 protocol=TCP 
netsh ipsec static add rule name=AllowOpenSomePort policy=safedog  filterlist=OpenSomePort filteraction=Permit 

REM ����ĳЩip���Է���ĳЩ�˿� 
netsh ipsec static add filterlist name=SomeIPSomePort 
netsh ipsec static add filter filterlist=SomeIPSomePort srcaddr=Me dstaddr=Any dstport=80 protocol=TCP 
netsh ipsec static add filter filterlist=SomeIPSomePort srcaddr=61.128.128.68 dstaddr=Me dstport=1433 protocol=TCP 
netsh ipsec static add rule name=AllowSomeIPSomePort policy=safedog  filterlist=SomeIPSomePort filteraction=Permit 

