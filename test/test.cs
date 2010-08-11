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
        set("short", "东门");
       /* set("long",
"这是东城门，城门正上方刻着“"+YEL"东门"NOR+"”两个楷书大字。城墙上贴着几张官府
"+HIW"告示"NOR+"(gaoshi)。官兵们警惕地注视着过往行人，好不威严。这里车水马龙，到处
都是行人。虽然这里是城门口，但是小贩们也来到这里，不远处，有卖水的、卖
苹果的、橘子香瓜、零食小点，真是要什么有什么。一条笔直的青石板大道向东
西两边延伸。东边是郊外，北面有一片茂盛的青竹林。\n"
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
