// this source is to test the grammer and vm(interpretor)
//#include "test.h"
//
use cgi;
use object;
use test::a::testobj;
load baselib;
//inherit object;
int a;

/*test comments*/
void main(string j, string p)
{
	string b;
	b = "daff";
	my a = "ha";
	main("","");
	var t = new test::testobj();
}

void set(string name, var obj){
	//this->{name} = obj;
	int a;
}

void create()
{
	int l = 110;
	test0();
	test1(111);
// = 123456;
	putl(l);
	putl(324);
	put_str("1234567890\r\nok\r\n");
	put_str("afa\r\n");
        set("short", "����");
       /* set("long",
"���Ƕ����ţ��������Ϸ����š�"+YEL"����"NOR+"������������֡���ǽ�����ż��Źٸ�
"+HIW"��ʾ"NOR+"(gaoshi)���ٱ��Ǿ����ע���Ź������ˣ��ò����ϡ����ﳵˮ����������
�������ˡ���Ȼ�����ǳ��ſڣ�����С����Ҳ���������Զ��������ˮ�ġ���
ƻ���ġ�������ϡ���ʳС�㣬����Ҫʲô��ʲô��һ����ֱ����ʯ������
���������졣�����ǽ��⣬������һƬïʢ�������֡�\n"
        );*/
        set("outdoors", "city");

      //  set("item_desc", {
       //         "gaoshi" :  set 
       // });
        set("exits", {
                "east" : "/d/taishan/yidao",
                "north" : "/d/gaibang/ml1",
                "west" : "dongdajie3",
                "south" : "dongjiao3"
        });

        set("objects", {
                "npc/wujiang" : 1,
                "npc/bing" : 2,
                "/clone/npc/zuo" : 1
        });

        set("cost", 1);
        //setup();
}
