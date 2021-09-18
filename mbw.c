/*
 * vim: ai ts=4 sts=4 sw=4 cinoptions=>4 expandtab
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#include "list.h"

#include "json.h"


char *_gjson;


char g_decode_json[2048];
int  g_decode_index;


static void process_value_dp( json_value* value, int depth );       



#define print_log(...)      ((void)printf(__VA_ARGS__))
//#define json_printf(...)    ((void)__android_log_print(ANDROID_LOG_INFO, "json", __VA_ARGS__))

#define json_printf(...) 





static void get_depth_shift(int depth, char *_depth)
{
        int j = 0;
        for (j=0; j < depth; j++) {
            //json_printf(" ");
        	_depth[j] = ' ';
        }
		_depth[j] = 0;

        //json_printf("%s", _depth);
}


static void process_value(json_value* value, int depth);

static void process_object(json_value* value, int depth)
{
        int length, x;
        if (value == NULL) {
                return;
        }
        length = value->u.object.length;
        for (x = 0; x < length; x++) {
				char _depth[256] = {0};
                //print_depth_shift(depth);
				get_depth_shift(depth, _depth);
                json_printf("%sobject[%d].name = %s", _depth, x, value->u.object.values[x].name);
                process_value(value->u.object.values[x].value, depth+1);
        }
}

void add_decode_buff( char *in_str )
{
    int buff_len;
    buff_len = strlen(in_str);
        
	if( (buff_len+g_decode_index) >= sizeof(g_decode_json) ){
        return; 
    }
    memcpy( &g_decode_json[g_decode_index], in_str, buff_len );
    g_decode_index = g_decode_index + buff_len;

	//printf("g_decode_json = %s\n", g_decode_json);

	//printf("g_decode_jsonxcv.\n\n");
	
}

void init_decode_buff( void )
{
    memset( &g_decode_json[0], 0x00, sizeof(g_decode_json) );
	g_decode_index = 0x00;
}

void end_of_decode_buff(void)
{         
    int buff_len;
    buff_len = strlen( g_decode_json );
    if( buff_len == 1 ){
		g_decode_json[1] = '}';
        return; 
    }
	if( g_decode_json[buff_len-1] == ',' ){
        g_decode_json[buff_len-1] = '}';
    }
}







static void process_object_dp(json_value* value, int depth)
{
    int length, x;
    char tmp_buff[256];
    int buff_len;
    if( value == NULL ){
        return;
    }
    length = value->u.object.length;
    print_log( "object length = %d\n\n", length );
    //遍历对象的所有成员 如果成员还是对象 会递归调用
    for( x=0; x<length; x++ ){
         char _depth[256] = {0};
         //print_depth_shift(depth);
         get_depth_shift(depth, _depth);
         json_printf("%sobject[%d].name = %s\n", _depth, x, value->u.object.values[x].name);
         print_log("%sobject[%d].name = %s\n", _depth, x, value->u.object.values[x].name);
         memset( tmp_buff, 0x00, sizeof(tmp_buff) );
         sprintf( tmp_buff, "\"%s\":", value->u.object.values[x].name );
            
		 //printf("tmp_buff = %s\n", tmp_buff);
         add_decode_buff( tmp_buff );  
		 //把键值输出
         process_value_dp(value->u.object.values[x].value, depth+4);
    }
}









static void process_array(json_value* value, int depth)
{
        int length, x;
        if (value == NULL) {
                return;
        }
        length = value->u.array.length;
        json_printf("array");
        for (x = 0; x < length; x++) {
                process_value(value->u.array.values[x], depth);
        }
}

static void process_value(json_value* value, int depth)
{
        int j;
		char _depth[256] = {0};
        if (value == NULL) {
                return;
        }
        if (value->type != json_object) {
			//char _depth[256];
            get_depth_shift(depth, _depth);
            //print_depth_shift(depth);
        }
        switch (value->type) {
                case json_none:
                        json_printf("%s%s", _depth, "none");
                        break;
                case json_object:
                        process_object(value, depth+1);
                        break;
                case json_array:
                        process_array(value, depth+1);
                        break;
                case json_integer:
						json_printf("%sint: %d", _depth, value->u.integer);
                        //printf("int: %10 \n", value->u.integer);
						//printf("int: %d \n", value->u.integer);
                        break;
                case json_double:
                        json_printf("%sdouble: %f", _depth, value->u.dbl);
                        break;
                case json_string:
                        json_printf("%sstring: %s", _depth, value->u.string.ptr);
                        break;
                case json_boolean:
                        json_printf("%sbool: %d", _depth, value->u.boolean);
                        break;
        }
}


static void process_value_dp( json_value* value, int depth )         
{
    int j;
    char _depth[256] = {0};
    char tmp_buff[2560] = {0x00};
    int buff_len;	
    if( value == NULL ){
        return;
    }
    if( value->type != json_object ){
        //char _depth[256];
        get_depth_shift(depth, _depth);
        //print_depth_shift(depth);
    }
    //print_log( "value->type = %d\n", value->type );	
    switch( value->type ) {
      case json_none:
        json_printf("%s%s", _depth, "none");
      break;
      case json_object:
        process_object_dp(value, depth+1);
      break;
      case json_array:
        process_array(value, depth+1);
      break;
      case json_integer:
        json_printf("%sint: %d", _depth, value->u.integer);
        print_log("%sint: %d", _depth, value->u.integer);
        memset( tmp_buff, 0x00, sizeof(tmp_buff) );
        sprintf( tmp_buff, "%d,", value->u.integer );
        add_decode_buff(tmp_buff);
        //printf("int: %10 \n", value->u.integer);
        //printf("int: %d \n", value->u.integer);
      break;
      case json_double:
        json_printf("%sdouble: %f", _depth, value->u.dbl);
      break;
      case json_string:
        json_printf("%sstring: %s\n", _depth, value->u.string.ptr);
        print_log("%sstring: %s\n", _depth, value->u.string.ptr);
        memset( tmp_buff, 0x00, sizeof(tmp_buff) );
        sprintf( tmp_buff, "\"%s\",", value->u.string.ptr );
        add_decode_buff(tmp_buff);	
		//把相应的value输出
      break;
      case json_boolean:
        json_printf("%sbool: %d", _depth, value->u.boolean);
      break;
    }
}


static void getStringValue(json_value* value, int depth, char *_value)
{
	int j;
	char _depth[256] = { 0 };
	if (value == NULL) {
		return;
	}
	if (value->type != json_object) {
		//char _depth[256];
		get_depth_shift(depth, _depth);
		//print_depth_shift(depth);
	}
	switch (value->type) {
	case json_none:
		json_printf("%s%s", _depth, "none");
		break;
	case json_object:
		process_object(value, depth + 1);
		break;
	case json_array:
		process_array(value, depth + 1);
		break;
	case json_integer:
		//json_printf("%sint: %d", _depth, value->u.integer);
		sprintf(_value, "%d", value->u.integer);
		break;
	case json_double:
		//json_printf("%sdouble: %f", _depth, value->u.dbl);
		sprintf(_value, "%f", value->u.dbl);
		break;
	case json_string:
		//json_printf("%sstring: %s", _depth, value->u.string.ptr);
		sprintf(_value, "%s", value->u.string.ptr);
		break;
	case json_boolean:
		//json_printf("%sbool: %d", _depth, value->u.boolean);
		sprintf(_value, "%d", value->u.boolean);
		break;
	}
}




//处理 获取 键对应的值 即key --> value
static void getStringValue_dp( json_value* value, int depth, char *_value )                   
{
	int j;
	char _depth[256] = { 0 };
	if (value == NULL) {
		return;
	}
	if (value->type != json_object) {
		//char _depth[256];
		get_depth_shift(depth, _depth);
		//print_depth_shift(depth);
	}
	
	print_log( "getStringValue_dp value->type = %d\n", value->type );
	switch (value->type) {
	case json_none:
		json_printf("%s%s", _depth, "none");
		break;
	case json_object:
		//如果值是对象的话 处理对象
		process_object_dp(value, depth + 1);
		break;
	case json_array:
		process_array(value, depth + 1);
		break;
	case json_integer:
		//json_printf("%sint: %d", _depth, value->u.integer);
		sprintf(_value, "%d", value->u.integer);
		printf("sdfd %s\n",_value );
		break;
	case json_double:
		//json_printf("%sdouble: %f", _depth, value->u.dbl);
		sprintf(_value, "%f", value->u.dbl);
		break;
	case json_string:
		//json_printf("%sstring: %s", _depth, value->u.string.ptr);
		sprintf(_value, "%s", value->u.string.ptr);
		break;
	case json_boolean:
		//json_printf("%sbool: %d", _depth, value->u.boolean);
		sprintf(_value, "%d", value->u.boolean);
		break;
	}
}








static void getObjectValue(json_value* value, int depth, char *_name, char *_value)
{
	int length, x;
	if (value == NULL) {
		return;
	}
	length = value->u.object.length;

	//print_log("_name = %s", _name);
	//print_log("_value = %s", _value);
	//print_log("depth = %d", depth);

	//print_log("length = %d", length);
	

	
	for (x = 0; x < length; x++) {
		char _depth[256] = { 0 };
		//print_depth_shift(depth);
		get_depth_shift(depth, _depth);
		
		//json_printf("%sobject[%d].name = %s", _depth, x, value->u.object.values[x].name);
		//process_value(value->u.object.values[x].value, depth + 1);

		if (strcmp(value->u.object.values[x].name, _name) == 0)
		{
			//strcpy(_value, value->u.object.values[x].value);
			getStringValue(value->u.object.values[x].value, depth + 1, _value);
			return;
		}
	}
}

static void getObjectValue_dp(json_value* value, int depth, char *_name, char *_value)
{                 
    int length, x;
    if( value == NULL ){
        return;
    }
    length = value->u.object.length;
    //在这一级有几个对象(平级 同一级别)  
    print_log("_name = %s\n", _name);
    print_log("_value = %s\n", _value);
    print_log("depth = %d\n", depth);
    print_log("length = %d\n", length);
                                            	
    for( x=0; x<length; x++ ){
         //遍历本级的所有对象                   
         char _depth[256] = { 0x00 };
         //print_depth_shift(depth);
         get_depth_shift(depth, _depth);
            
         //json_printf("%sobject[%d].name = %s", _depth, x, value->u.object.values[x].name);
         //process_value(value->u.object.values[x].value, depth + 1);
         print_log("%sobject[%d].name = %s\n", _depth, x, value->u.object.values[x].name);
         if( strcmp(value->u.object.values[x].name, _name) == 0 ){
             //找到了要匹配的键值 --> key		 	
             //strcpy(_value, value->u.object.values[x].value);
             getStringValue_dp(value->u.object.values[x].value, depth + 1, _value);
             //获取相应的值-->value
             return;
             //找到即可返回了
         }
    }
}







int getJsonValue(char *_json_str, char *_name, char *_value)
{
	//printf("%s\n", _json_str);
	//print_buf("--------------------------------\n\n\n\n", NULL, 0);
	//print_buf("--------------------------------decode JSON\n\n", NULL, 0);

	json_char *json = (json_char*)_json_str;

	json_value *value = json_parse(json, strlen(_json_str));
	if( value == NULL ){
        print_log("Unable to parse data");
        return 1;
    }

	getObjectValue(value, 0, _name, _value);

	//print_log("_json_str = %s", _json_str);
	//print_log("_name = %s", _name);
	//print_log("_value = %s", _value);

	

	json_value_free(value);
	return 0;
}



int getJsonValue_dp( char *_json_str, char *_name, char *_value )
{       
    //printf("%s\n", _json_str);
    //print_buf("--------------------------------\n\n\n\n", NULL, 0);
    //print_buf("--------------------------------decode JSON\n\n", NULL, 0);
    json_char *json = (json_char*)_json_str;
    json_value *value = json_parse(json, strlen(_json_str));
    //解析json到value
    if( value == NULL ){
        print_log("Unable to parse data\n");
        return 1;
    }
                         	
    getObjectValue_dp(value, 0, _name, _value);
    //获取键值为_name的对象
          	
    //print_log("_json_str = %s\n", _json_str);
    //print_log("_name = %s\n", _name);
    //print_log("_value = %s\n", _value);
    json_value_free(value);
    return 0;
}











int main( int argc, char **argv )
{
    int i;
    unsigned long int *tableA;
	int arg[8];
	char _value[1024] = {0};
	int ret;
	
	//_gjson = "{\"status\":123,\"remark\":\"request success!\",\"data\":\
//{\"id\":251},\"type\":\"\",\"filePath\":\"\"}";

	//printf("json = %s\n", _gjson );


	_gjson = "{\"status\": 9123,\"remark\":\"computed finish!\",\
\"data\":{\"data\":\"[\\\"1122334455\\\"]\",\"result\":\"5566778899\",\"psn\":\"10003047\"},\"type\":\"\",\"filePath\":\"\"}";
	

    printf("json = %s\n", _gjson );
    init_decode_buff();
    add_decode_buff( "{" );
    ret = getJsonValue_dp(_gjson, "data", _value);
    if( ret == 0x00 ){    
        end_of_decode_buff();
        //srv_len = strlen(g_decode_json);
    }					
    //printf("data 2= %s\n", _value);
    printf("\nparse result = %s.\n\n", g_decode_json);
}

























//scp -r  dai@192.168.0.107:/home/dai/mbw/* ./


