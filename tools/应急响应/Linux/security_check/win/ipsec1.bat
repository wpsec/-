Netsh ipsec static add policy name = Ĭ�ϲ�������

Netsh ipsec static add filteraction name = ��ֹ���� action = block

Netsh ipsec static add filteraction name = ������� action = permit

Netsh ipsec static add filterlist name = �����б�

Netsh ipsec static add filterlist name = ��ֹ�б�

Netsh ipsec static add filter filterlist = �����б�1 srcaddr = 203.86.32.248 dstaddr = me dstport = 3389 description = ����1���� protocol = TCP mirrored = yes

Netsh ipsec static add filter filterlist = �����б�2 srcaddr = 203.86.31.0 srcmask = 255.255.255.0  dstaddr = 60.190.145.9 dstport = 0 description = ����2���� protocol = any mirrored = yes

Netsh ipsec static add rule name = �ɷ��ʵ��ն˲��Թ��� Policy = Ĭ�ϲ������� filterlist = �����б�1 filteraction = ��ֹ����

Netsh ipsec static add rule name = �ɷ��ʵ��ն˲��Թ��� Policy = Ĭ�ϲ������� filterlist = �����б�2 filteraction = ��ֹ����

netsh ipsec static set policy name = Ĭ�ϲ������� assign = y

pause