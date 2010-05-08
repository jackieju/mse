// this source is to test the grammer and vm(interpretor)
//#include "test.h"
//
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
	test();
// = 123456;
	putl(l);
	putl(324);
	put_str("\r\nok\r\n");
	put_str("afa\r\n");
        set("short", "¶«ÃÅ");
       /* set("long",
"ÕâÊÇ¶«³ÇÃÅ£¬³ÇÃÅÕýÉÏ·½¿Ì×Å¡°"+YEL"¶«ÃÅ"NOR+"¡±Á½¸ö¿¬Êé´ó×Ö¡£³ÇÇ½ÉÏÌù×Å¼¸ÕÅ¹Ù¸®
"+HIW"¸æÊ¾"NOR+"(gaoshi)¡£¹Ù±øÃÇ¾¯ÌèµØ×¢ÊÓ×Å¹ýÍùÐÐÈË£¬ºÃ²»ÍþÑÏ¡£ÕâÀï³µË®ÂíÁú£¬µ½´¦
¶¼ÊÇÐÐÈË¡£ËäÈ»ÕâÀïÊÇ³ÇÃÅ¿Ú£¬µ«ÊÇÐ¡··ÃÇÒ²À´µ½ÕâÀï£¬²»Ô¶´¦£¬ÓÐÂôË®µÄ¡¢Âô
Æ»¹ûµÄ¡¢éÙ×ÓÏã¹Ï¡¢ÁãÊ³Ð¡µã£¬ÕæÊÇÒªÊ²Ã´ÓÐÊ²Ã´¡£Ò»Ìõ±ÊÖ±µÄÇàÊ¯°å´óµÀÏò¶«
Î÷Á½±ßÑÓÉì¡£¶«±ßÊÇ½¼Íâ£¬±±ÃæÓÐÒ»Æ¬Ã¯Ê¢µÄÇàÖñÁÖ¡£\n"
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
