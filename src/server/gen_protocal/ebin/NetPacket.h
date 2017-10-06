#pragma once
    #include"ByteArray.h"
    #include"BaseType.h"
    #include"INetPacket.h"
    #include"NetMsgType.h"
    #include<string>
    #include<vector>
    
struct req_ver_validate : public INetPacket
{
	int ver;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(ver);
	}

	void decode(ByteArray &byteArray)
	{
		ver = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_ver_validate);
		encode(byteArray);
	}
};

struct notify_ver_validate : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_ver_validate);
		encode(byteArray);
	}
};

struct stime : public INetPacket
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(year);
		byteArray.write_int(month);
		byteArray.write_int(day);
		byteArray.write_int(hour);
		byteArray.write_int(minute);
		byteArray.write_int(second);
	}

	void decode(ByteArray &byteArray)
	{
		year = byteArray.read_int();
		month = byteArray.read_int();
		day = byteArray.read_int();
		hour = byteArray.read_int();
		minute = byteArray.read_int();
		second = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_stime);
		encode(byteArray);
	}
};

struct point : public INetPacket
{
	float x;
	float y;
	float z;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_float(x);
		byteArray.write_float(y);
		byteArray.write_float(z);
	}

	void decode(ByteArray &byteArray)
	{
		x = byteArray.read_float();
		y = byteArray.read_float();
		z = byteArray.read_float();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_point);
		encode(byteArray);
	}
};

struct grid_pos : public INetPacket
{
	int x;
	int y;
	int z;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(x);
		byteArray.write_int(y);
		byteArray.write_int(z);
	}

	void decode(ByteArray &byteArray)
	{
		x = byteArray.read_int();
		y = byteArray.read_int();
		z = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_grid_pos);
		encode(byteArray);
	}
};

struct item : public INetPacket
{
	uint64 instance_id;
	int template_id;
	stime del_time;
	int decoration_val;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(template_id);
		del_time.encode(byteArray);

		byteArray.write_int(decoration_val);
	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		template_id = byteArray.read_int();
		del_time.decode(byteArray);

		decoration_val = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_item);
		encode(byteArray);
	}
};

struct player_basic_data : public INetPacket
{
	std::string account;
	std::string username;
	int sex;
	int skin_color;
	int hair;
	int face;
	int beard;
	int age;
	int level;
	float online_time;
	int health_status;
	std::string personality_sign;
	std::string personality_describe;
	int hair_color;
	stime last_login_time;
	int gm_enabled;
	stime logout_time;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(username);
		byteArray.write_int(sex);
		byteArray.write_int(skin_color);
		byteArray.write_int(hair);
		byteArray.write_int(face);
		byteArray.write_int(beard);
		byteArray.write_int(age);
		byteArray.write_int(level);
		byteArray.write_float(online_time);
		byteArray.write_int(health_status);
		byteArray.write_string(personality_sign);
		byteArray.write_string(personality_describe);
		byteArray.write_int(hair_color);
		last_login_time.encode(byteArray);

		byteArray.write_int(gm_enabled);
		logout_time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		username = byteArray.read_string();
		sex = byteArray.read_int();
		skin_color = byteArray.read_int();
		hair = byteArray.read_int();
		face = byteArray.read_int();
		beard = byteArray.read_int();
		age = byteArray.read_int();
		level = byteArray.read_int();
		online_time = byteArray.read_float();
		health_status = byteArray.read_int();
		personality_sign = byteArray.read_string();
		personality_describe = byteArray.read_string();
		hair_color = byteArray.read_int();
		last_login_time.decode(byteArray);

		gm_enabled = byteArray.read_int();
		logout_time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_basic_data);
		encode(byteArray);
	}
};

struct player_property : public INetPacket
{
	std::string account;
	int player_clean;
	int player_health;
	int player_charm;
	int active_value;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(player_clean);
		byteArray.write_int(player_health);
		byteArray.write_int(player_charm);
		byteArray.write_int(active_value);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		player_clean = byteArray.read_int();
		player_health = byteArray.read_int();
		player_charm = byteArray.read_int();
		active_value = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_property);
		encode(byteArray);
	}
};

struct pack_grid : public INetPacket
{
	int count;
	int lock;
	item item_data;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(count);
		byteArray.write_int(lock);
		item_data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		count = byteArray.read_int();
		lock = byteArray.read_int();
		item_data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_pack_grid);
		encode(byteArray);
	}
};

struct player_body : public INetPacket
{
	std::string owner;
	std::vector<pack_grid> grid_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(owner);
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		owner = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_body);
		encode(byteArray);
	}
};

struct player_pack : public INetPacket
{
	std::string owner;
	std::vector<pack_grid> grid_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(owner);
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		owner = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_pack);
		encode(byteArray);
	}
};

struct player_info : public INetPacket
{
	player_basic_data basic_data;
	std::string scenename;
	void encode(ByteArray &byteArray)
	{
		basic_data.encode(byteArray);

		byteArray.write_string(scenename);
	}

	void decode(ByteArray &byteArray)
	{
		basic_data.decode(byteArray);

		scenename = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_info);
		encode(byteArray);
	}
};

struct friend_info : public INetPacket
{
	player_basic_data basic_data;
	int is_online;
	void encode(ByteArray &byteArray)
	{
		basic_data.encode(byteArray);

		byteArray.write_int(is_online);
	}

	void decode(ByteArray &byteArray)
	{
		basic_data.decode(byteArray);

		is_online = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_friend_info);
		encode(byteArray);
	}
};

struct mail_info : public INetPacket
{
	uint64 id;
	int type;
	int read_status;
	int remain_time;
	std::string sender_name;
	std::string sender_account;
	std::string title;
	std::vector<pack_grid> items;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_int(type);
		byteArray.write_int(read_status);
		byteArray.write_int(remain_time);
		byteArray.write_string(sender_name);
		byteArray.write_string(sender_account);
		byteArray.write_string(title);
		byteArray.write_uint16(items.size());
		for(size_t i=0; i<items.size();i++)
		{
			items[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		type = byteArray.read_int();
		read_status = byteArray.read_int();
		remain_time = byteArray.read_int();
		sender_name = byteArray.read_string();
		sender_account = byteArray.read_string();
		title = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		items.resize(size);
		for(int i=0; i<size;i++)
		{
			items[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_mail_info);
		encode(byteArray);
	}
};

struct npc_info : public INetPacket
{
	int npc_id;
	int body;
	int head;
	int hair;
	int equip1;
	int equip2;
	int skeleton;
	std::string npc_name;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(npc_id);
		byteArray.write_int(body);
		byteArray.write_int(head);
		byteArray.write_int(hair);
		byteArray.write_int(equip1);
		byteArray.write_int(equip2);
		byteArray.write_int(skeleton);
		byteArray.write_string(npc_name);
	}

	void decode(ByteArray &byteArray)
	{
		npc_id = byteArray.read_int();
		body = byteArray.read_int();
		head = byteArray.read_int();
		hair = byteArray.read_int();
		equip1 = byteArray.read_int();
		equip2 = byteArray.read_int();
		skeleton = byteArray.read_int();
		npc_name = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_npc_info);
		encode(byteArray);
	}
};

struct npc_map_mapping_info : public INetPacket
{
	int id;
	int npc_id;
	std::string npc_name;
	point pos;
	int script_id;
	int action;
	std::string npc_key;
	int towards;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
		byteArray.write_int(npc_id);
		byteArray.write_string(npc_name);
		pos.encode(byteArray);

		byteArray.write_int(script_id);
		byteArray.write_int(action);
		byteArray.write_string(npc_key);
		byteArray.write_int(towards);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
		npc_id = byteArray.read_int();
		npc_name = byteArray.read_string();
		pos.decode(byteArray);

		script_id = byteArray.read_int();
		action = byteArray.read_int();
		npc_key = byteArray.read_string();
		towards = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_npc_map_mapping_info);
		encode(byteArray);
	}
};

struct npc_command : public INetPacket
{
	int template_id;
	uint64 unique_id;
	int page_index;
	int command_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(template_id);
		byteArray.write_uint64(unique_id);
		byteArray.write_int(page_index);
		byteArray.write_int(command_index);
	}

	void decode(ByteArray &byteArray)
	{
		template_id = byteArray.read_int();
		unique_id = byteArray.read_uint64();
		page_index = byteArray.read_int();
		command_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_npc_command);
		encode(byteArray);
	}
};

struct furniture_position : public INetPacket
{
	int position_index;
	int is_used;
	std::string used_account;
	int status;
	int func_id;
	stime use_time;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(position_index);
		byteArray.write_int(is_used);
		byteArray.write_string(used_account);
		byteArray.write_int(status);
		byteArray.write_int(func_id);
		use_time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		position_index = byteArray.read_int();
		is_used = byteArray.read_int();
		used_account = byteArray.read_string();
		status = byteArray.read_int();
		func_id = byteArray.read_int();
		use_time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_furniture_position);
		encode(byteArray);
	}
};

struct player_task : public INetPacket
{
	uint64 id;
	std::string account;
	int task_id;
	int step_index;
	int track;
	int status;
	stime create_date;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_string(account);
		byteArray.write_int(task_id);
		byteArray.write_int(step_index);
		byteArray.write_int(track);
		byteArray.write_int(status);
		create_date.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		account = byteArray.read_string();
		task_id = byteArray.read_int();
		step_index = byteArray.read_int();
		track = byteArray.read_int();
		status = byteArray.read_int();
		create_date.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_task);
		encode(byteArray);
	}
};

struct eq_user_info : public INetPacket
{
	std::string full_name;
	std::string nick_name;
	std::string birthday;
	std::string phone;
	std::string url;
	std::string email;
	std::string country;
	std::string region;
	std::string city;
	std::string street;
	std::string org_name;
	std::string org_unit;
	std::string title;
	std::string role;
	std::string desc;
	std::string photo_type;
	std::string photo_data;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(full_name);
		byteArray.write_string(nick_name);
		byteArray.write_string(birthday);
		byteArray.write_string(phone);
		byteArray.write_string(url);
		byteArray.write_string(email);
		byteArray.write_string(country);
		byteArray.write_string(region);
		byteArray.write_string(city);
		byteArray.write_string(street);
		byteArray.write_string(org_name);
		byteArray.write_string(org_unit);
		byteArray.write_string(title);
		byteArray.write_string(role);
		byteArray.write_string(desc);
		byteArray.write_string(photo_type);
		byteArray.write_string(photo_data);
	}

	void decode(ByteArray &byteArray)
	{
		full_name = byteArray.read_string();
		nick_name = byteArray.read_string();
		birthday = byteArray.read_string();
		phone = byteArray.read_string();
		url = byteArray.read_string();
		email = byteArray.read_string();
		country = byteArray.read_string();
		region = byteArray.read_string();
		city = byteArray.read_string();
		street = byteArray.read_string();
		org_name = byteArray.read_string();
		org_unit = byteArray.read_string();
		title = byteArray.read_string();
		role = byteArray.read_string();
		desc = byteArray.read_string();
		photo_type = byteArray.read_string();
		photo_data = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_eq_user_info);
		encode(byteArray);
	}
};

struct offline_invite_record : public INetPacket
{
	int invite_type;
	std::string invitor;
	std::string invitor_name;
	stime date;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(invite_type);
		byteArray.write_string(invitor);
		byteArray.write_string(invitor_name);
		date.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		invite_type = byteArray.read_int();
		invitor = byteArray.read_string();
		invitor_name = byteArray.read_string();
		date.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_offline_invite_record);
		encode(byteArray);
	}
};

struct friend_setting_record : public INetPacket
{
	int accept_all_invite;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(accept_all_invite);
	}

	void decode(ByteArray &byteArray)
	{
		accept_all_invite = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_friend_setting_record);
		encode(byteArray);
	}
};

struct magic_box : public INetPacket
{
	int type;
	uint64 inst_id;
	grid_pos pos;
	int graphic_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_uint64(inst_id);
		pos.encode(byteArray);

		byteArray.write_int(graphic_id);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		inst_id = byteArray.read_uint64();
		pos.decode(byteArray);

		graphic_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_magic_box);
		encode(byteArray);
	}
};

struct gift_box : public INetPacket
{
	uint64 inst_id;
	std::string sender;
	std::string sender_name;
	std::string receiver;
	std::string receiver_name;
	std::string name;
	std::vector<pack_grid> grid_vec;
	int box_id;
	int card_id;
	std::string comment;
	grid_pos pos;
	stime date;
	int is_anonymous;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
		byteArray.write_string(sender);
		byteArray.write_string(sender_name);
		byteArray.write_string(receiver);
		byteArray.write_string(receiver_name);
		byteArray.write_string(name);
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

		byteArray.write_int(box_id);
		byteArray.write_int(card_id);
		byteArray.write_string(comment);
		pos.encode(byteArray);

		date.encode(byteArray);

		byteArray.write_int(is_anonymous);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
		sender = byteArray.read_string();
		sender_name = byteArray.read_string();
		receiver = byteArray.read_string();
		receiver_name = byteArray.read_string();
		name = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

		box_id = byteArray.read_int();
		card_id = byteArray.read_int();
		comment = byteArray.read_string();
		pos.decode(byteArray);

		date.decode(byteArray);

		is_anonymous = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_gift_box);
		encode(byteArray);
	}
};

struct flower_log : public INetPacket
{
	stime time;
	std::string who;
	void encode(ByteArray &byteArray)
	{
		time.encode(byteArray);

		byteArray.write_string(who);
	}

	void decode(ByteArray &byteArray)
	{
		time.decode(byteArray);

		who = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_flower_log);
		encode(byteArray);
	}
};

struct egg_log : public INetPacket
{
	stime time;
	std::string who;
	void encode(ByteArray &byteArray)
	{
		time.encode(byteArray);

		byteArray.write_string(who);
	}

	void decode(ByteArray &byteArray)
	{
		time.decode(byteArray);

		who = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_egg_log);
		encode(byteArray);
	}
};

struct req_login : public INetPacket
{
	std::string account;
	std::string password;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(password);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		password = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_login);
		encode(byteArray);
	}
};

struct notify_login_result : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_login_result);
		encode(byteArray);
	}
};

struct notify_repeat_login : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_repeat_login);
		encode(byteArray);
	}
};

struct req_get_roles : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_roles);
		encode(byteArray);
	}
};

struct notify_get_roles_result : public INetPacket
{
	std::vector<player_info> player;
	std::string nick_name;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(player.size());
		for(size_t i=0; i<player.size();i++)
		{
			player[i].encode(byteArray);
		}

		byteArray.write_string(nick_name);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		player.resize(size);
		for(int i=0; i<size;i++)
		{
			player[i].decode(byteArray);
		}
		}

		nick_name = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_roles_result);
		encode(byteArray);
	}
};

struct req_create_role : public INetPacket
{
	player_basic_data basic_data;
	std::vector<item> equips;
	void encode(ByteArray &byteArray)
	{
		basic_data.encode(byteArray);

		byteArray.write_uint16(equips.size());
		for(size_t i=0; i<equips.size();i++)
		{
			equips[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		basic_data.decode(byteArray);

		{
		UINT16 size = byteArray.read_uint16();
		equips.resize(size);
		for(int i=0; i<size;i++)
		{
			equips[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_create_role);
		encode(byteArray);
	}
};

struct notify_create_role_result : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_create_role_result);
		encode(byteArray);
	}
};

struct req_enter_scene : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_enter_scene);
		encode(byteArray);
	}
};

struct notify_enter_scene_result : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_enter_scene_result);
		encode(byteArray);
	}
};

struct notify_body_data : public INetPacket
{
	std::vector<pack_grid> grid_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_body_data);
		encode(byteArray);
	}
};

struct req_enter_player_house : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_enter_player_house);
		encode(byteArray);
	}
};

struct req_enter_common_scene : public INetPacket
{
	int scene_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(scene_id);
	}

	void decode(ByteArray &byteArray)
	{
		scene_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_enter_common_scene);
		encode(byteArray);
	}
};

struct notify_enter_common_scene : public INetPacket
{
	int scene_id;
	point enter_pos;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(scene_id);
		enter_pos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		scene_id = byteArray.read_int();
		enter_pos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_enter_common_scene);
		encode(byteArray);
	}
};

struct notify_enter_player_house : public INetPacket
{
	std::string account;
	point enter_pos;
	int house_tplt_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		enter_pos.encode(byteArray);

		byteArray.write_int(house_tplt_id);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		enter_pos.decode(byteArray);

		house_tplt_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_enter_player_house);
		encode(byteArray);
	}
};

struct req_teleport : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_teleport);
		encode(byteArray);
	}
};

struct req_open_change_scene_guide : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_open_change_scene_guide);
		encode(byteArray);
	}
};

struct notify_open_change_scene_guide : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_open_change_scene_guide);
		encode(byteArray);
	}
};

struct req_close_change_scene_guide : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_close_change_scene_guide);
		encode(byteArray);
	}
};

struct notify_close_change_scene_guide : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_close_change_scene_guide);
		encode(byteArray);
	}
};

struct notify_finish_enter_scene : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_finish_enter_scene);
		encode(byteArray);
	}
};

struct req_kick_guest : public INetPacket
{
	std::string target_player;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(target_player);
	}

	void decode(ByteArray &byteArray)
	{
		target_player = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_kick_guest);
		encode(byteArray);
	}
};

struct notify_other_player_data : public INetPacket
{
	player_info player;
	player_property property;
	point curr_pos;
	point dest_pos;
	uint64 used_furni_inst_id;
	int used_furni_func_id;
	int used_furni_index;
	void encode(ByteArray &byteArray)
	{
		player.encode(byteArray);

		property.encode(byteArray);

		curr_pos.encode(byteArray);

		dest_pos.encode(byteArray);

		byteArray.write_uint64(used_furni_inst_id);
		byteArray.write_int(used_furni_func_id);
		byteArray.write_int(used_furni_index);
	}

	void decode(ByteArray &byteArray)
	{
		player.decode(byteArray);

		property.decode(byteArray);

		curr_pos.decode(byteArray);

		dest_pos.decode(byteArray);

		used_furni_inst_id = byteArray.read_uint64();
		used_furni_func_id = byteArray.read_int();
		used_furni_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_player_data);
		encode(byteArray);
	}
};

struct notify_other_player_info : public INetPacket
{
	player_info player;
	player_property property;
	std::vector<pack_grid> body;
	void encode(ByteArray &byteArray)
	{
		player.encode(byteArray);

		property.encode(byteArray);

		byteArray.write_uint16(body.size());
		for(size_t i=0; i<body.size();i++)
		{
			body[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		player.decode(byteArray);

		property.decode(byteArray);

		{
		UINT16 size = byteArray.read_uint16();
		body.resize(size);
		for(int i=0; i<size;i++)
		{
			body[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_player_info);
		encode(byteArray);
	}
};

struct req_other_player_info : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_other_player_info);
		encode(byteArray);
	}
};

struct notify_player_enter_scene : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_enter_scene);
		encode(byteArray);
	}
};

struct notify_player_leave_scene : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_leave_scene);
		encode(byteArray);
	}
};

struct req_logout : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_logout);
		encode(byteArray);
	}
};

struct notify_player_data : public INetPacket
{
	player_basic_data basic_data;
	void encode(ByteArray &byteArray)
	{
		basic_data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		basic_data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_data);
		encode(byteArray);
	}
};

struct req_walk : public INetPacket
{
	std::vector<point> path;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(path.size());
		for(size_t i=0; i<path.size();i++)
		{
			path[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		path.resize(size);
		for(int i=0; i<size;i++)
		{
			path[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_walk);
		encode(byteArray);
	}
};

struct req_start_walk : public INetPacket
{
	point curr_pos;
	point dest_pos;
	int move_type;
	void encode(ByteArray &byteArray)
	{
		curr_pos.encode(byteArray);

		dest_pos.encode(byteArray);

		byteArray.write_int(move_type);
	}

	void decode(ByteArray &byteArray)
	{
		curr_pos.decode(byteArray);

		dest_pos.decode(byteArray);

		move_type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_walk);
		encode(byteArray);
	}
};

struct notify_start_walk : public INetPacket
{
	std::string account;
	point curr_pos;
	point dest_pos;
	int move_type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		curr_pos.encode(byteArray);

		dest_pos.encode(byteArray);

		byteArray.write_int(move_type);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		curr_pos.decode(byteArray);

		dest_pos.decode(byteArray);

		move_type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_walk);
		encode(byteArray);
	}
};

struct req_stop_walk : public INetPacket
{
	point pos;
	void encode(ByteArray &byteArray)
	{
		pos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		pos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_stop_walk);
		encode(byteArray);
	}
};

struct notify_stop_walk : public INetPacket
{
	std::string account;
	point pos;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		pos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		pos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_stop_walk);
		encode(byteArray);
	}
};

struct req_sync_position : public INetPacket
{
	point pos;
	void encode(ByteArray &byteArray)
	{
		pos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		pos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sync_position);
		encode(byteArray);
	}
};

struct req_walk_for_use_furniture : public INetPacket
{
	point curr_pos;
	point dest_pos;
	uint64 instance_id;
	int function_id;
	int move_type;
	void encode(ByteArray &byteArray)
	{
		curr_pos.encode(byteArray);

		dest_pos.encode(byteArray);

		byteArray.write_uint64(instance_id);
		byteArray.write_int(function_id);
		byteArray.write_int(move_type);
	}

	void decode(ByteArray &byteArray)
	{
		curr_pos.decode(byteArray);

		dest_pos.decode(byteArray);

		instance_id = byteArray.read_uint64();
		function_id = byteArray.read_int();
		move_type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_walk_for_use_furniture);
		encode(byteArray);
	}
};

struct req_add_friend : public INetPacket
{
	std::string friend_account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_add_friend);
		encode(byteArray);
	}
};

struct req_remove_friend : public INetPacket
{
	std::string friend_account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_remove_friend);
		encode(byteArray);
	}
};

struct notify_add_friend : public INetPacket
{
	std::string friend_account;
	friend_info friend_infos;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
		friend_infos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
		friend_infos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_add_friend);
		encode(byteArray);
	}
};

struct notify_remove_friend : public INetPacket
{
	std::string friend_account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_remove_friend);
		encode(byteArray);
	}
};

struct req_friend_list : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_friend_list);
		encode(byteArray);
	}
};

struct notify_friend_list : public INetPacket
{
	std::vector<friend_info> friend_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(friend_vec.size());
		for(size_t i=0; i<friend_vec.size();i++)
		{
			friend_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		friend_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			friend_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_friend_list);
		encode(byteArray);
	}
};

struct notify_friend_online : public INetPacket
{
	std::string friend_account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_friend_online);
		encode(byteArray);
	}
};

struct notify_friend_offline : public INetPacket
{
	std::string friend_account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_friend_offline);
		encode(byteArray);
	}
};

struct req_eq_users_info : public INetPacket
{
	std::vector<std::string> account_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(account_list.size());
		for(size_t i=0; i<account_list.size();i++)
		{
			byteArray.write_string(account_list[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		account_list.reserve(size);
		for(int i=0; i<size;i++)
		{
			account_list.push_back(byteArray.read_string());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_eq_users_info);
		encode(byteArray);
	}
};

struct notify_eq_user_info : public INetPacket
{
	std::string account;
	eq_user_info info;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		info.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		info.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_eq_user_info);
		encode(byteArray);
	}
};

struct notify_eq_user_presence : public INetPacket
{
	std::string account;
	std::string presence;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(presence);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		presence = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_eq_user_presence);
		encode(byteArray);
	}
};

struct notify_offline_invites : public INetPacket
{
	std::vector<offline_invite_record> inv_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(inv_list.size());
		for(size_t i=0; i<inv_list.size();i++)
		{
			inv_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		inv_list.resize(size);
		for(int i=0; i<size;i++)
		{
			inv_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_offline_invites);
		encode(byteArray);
	}
};

struct req_friend_setting : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_friend_setting);
		encode(byteArray);
	}
};

struct notify_friend_setting : public INetPacket
{
	friend_setting_record setting;
	void encode(ByteArray &byteArray)
	{
		setting.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		setting.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_friend_setting);
		encode(byteArray);
	}
};

struct req_friend_setting_accept_all : public INetPacket
{
	int accept_all;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(accept_all);
	}

	void decode(ByteArray &byteArray)
	{
		accept_all = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_friend_setting_accept_all);
		encode(byteArray);
	}
};

struct req_friend_setting_black_someone : public INetPacket
{
	std::string who;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(who);
	}

	void decode(ByteArray &byteArray)
	{
		who = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_friend_setting_black_someone);
		encode(byteArray);
	}
};

struct req_search_player_by_nick : public INetPacket
{
	std::string nick;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(nick);
	}

	void decode(ByteArray &byteArray)
	{
		nick = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_search_player_by_nick);
		encode(byteArray);
	}
};

struct req_search_player_by_mail : public INetPacket
{
	std::string mail;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(mail);
	}

	void decode(ByteArray &byteArray)
	{
		mail = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_search_player_by_mail);
		encode(byteArray);
	}
};

struct notify_search_player : public INetPacket
{
	std::vector<std::string> players;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(players.size());
		for(size_t i=0; i<players.size();i++)
		{
			byteArray.write_string(players[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		players.reserve(size);
		for(int i=0; i<size;i++)
		{
			players.push_back(byteArray.read_string());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_search_player);
		encode(byteArray);
	}
};

struct req_chat_around : public INetPacket
{
	std::string content;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(content);
	}

	void decode(ByteArray &byteArray)
	{
		content = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_chat_around);
		encode(byteArray);
	}
};

struct notify_chat_around : public INetPacket
{
	std::string account;
	std::string player_name;
	std::string content;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(player_name);
		byteArray.write_string(content);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		player_name = byteArray.read_string();
		content = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_chat_around);
		encode(byteArray);
	}
};

struct req_chat_tell : public INetPacket
{
	std::string target_player;
	std::string content;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(target_player);
		byteArray.write_string(content);
	}

	void decode(ByteArray &byteArray)
	{
		target_player = byteArray.read_string();
		content = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_chat_tell);
		encode(byteArray);
	}
};

struct notify_chat_tell : public INetPacket
{
	std::string speaker;
	std::string speaker_name;
	std::string content;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(speaker);
		byteArray.write_string(speaker_name);
		byteArray.write_string(content);
	}

	void decode(ByteArray &byteArray)
	{
		speaker = byteArray.read_string();
		speaker_name = byteArray.read_string();
		content = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_chat_tell);
		encode(byteArray);
	}
};

struct house_furniture : public INetPacket
{
	uint64 instance_id;
	int template_id;
	int x;
	int z;
	int height;
	int floor;
	int face;
	int item_tempid;
	int max_use_player;
	std::vector<furniture_position> position_list;
	std::vector<int> function_list;
	int use_sex;
	int permission;
	int use_type;
	int status_qty;
	stime del_time;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(template_id);
		byteArray.write_int(x);
		byteArray.write_int(z);
		byteArray.write_int(height);
		byteArray.write_int(floor);
		byteArray.write_int(face);
		byteArray.write_int(item_tempid);
		byteArray.write_int(max_use_player);
		byteArray.write_uint16(position_list.size());
		for(size_t i=0; i<position_list.size();i++)
		{
			position_list[i].encode(byteArray);
		}

		byteArray.write_uint16(function_list.size());
		for(size_t i=0; i<function_list.size();i++)
		{
			byteArray.write_int(function_list[i]);
		}

		byteArray.write_int(use_sex);
		byteArray.write_int(permission);
		byteArray.write_int(use_type);
		byteArray.write_int(status_qty);
		del_time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		template_id = byteArray.read_int();
		x = byteArray.read_int();
		z = byteArray.read_int();
		height = byteArray.read_int();
		floor = byteArray.read_int();
		face = byteArray.read_int();
		item_tempid = byteArray.read_int();
		max_use_player = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		position_list.resize(size);
		for(int i=0; i<size;i++)
		{
			position_list[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		function_list.reserve(size);
		for(int i=0; i<size;i++)
		{
			function_list.push_back(byteArray.read_int());
		}
		}

		use_sex = byteArray.read_int();
		permission = byteArray.read_int();
		use_type = byteArray.read_int();
		status_qty = byteArray.read_int();
		del_time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_house_furniture);
		encode(byteArray);
	}
};

struct room_tex : public INetPacket
{
	int room_id;
	int type;
	std::string tex;
	int floor_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(room_id);
		byteArray.write_int(type);
		byteArray.write_string(tex);
		byteArray.write_int(floor_id);
	}

	void decode(ByteArray &byteArray)
	{
		room_id = byteArray.read_int();
		type = byteArray.read_int();
		tex = byteArray.read_string();
		floor_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_room_tex);
		encode(byteArray);
	}
};

struct house_component_tex : public INetPacket
{
	std::string component_name;
	std::string new_tex;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(component_name);
		byteArray.write_string(new_tex);
	}

	void decode(ByteArray &byteArray)
	{
		component_name = byteArray.read_string();
		new_tex = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_house_component_tex);
		encode(byteArray);
	}
};

struct house_component_mesh : public INetPacket
{
	std::string component_name;
	std::string new_mesh;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(component_name);
		byteArray.write_string(new_mesh);
	}

	void decode(ByteArray &byteArray)
	{
		component_name = byteArray.read_string();
		new_mesh = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_house_component_mesh);
		encode(byteArray);
	}
};

struct house_visit_log : public INetPacket
{
	int type;
	stime datetime;
	std::string account;
	std::string username;
	int item_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		datetime.encode(byteArray);

		byteArray.write_string(account);
		byteArray.write_string(username);
		byteArray.write_int(item_id);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		datetime.decode(byteArray);

		account = byteArray.read_string();
		username = byteArray.read_string();
		item_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_house_visit_log);
		encode(byteArray);
	}
};

struct house_data : public INetPacket
{
	std::string owner;
	uint64 instance_id;
	int template_id;
	int furniture_permission;
	std::vector<house_furniture> furniture_vec;
	std::vector<room_tex> room_tex_vec;
	std::vector<house_component_tex> component_tex_vec;
	std::vector<house_component_mesh> component_mesh_vec;
	std::string welcome_words;
	int house_permission;
	std::vector<house_visit_log> visit_log;
	stime buy_date;
	int level;
	std::vector<gift_box> gift_box_vec;
	std::vector<magic_box> magic_box_vec;
	int house_clean;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(owner);
		byteArray.write_uint64(instance_id);
		byteArray.write_int(template_id);
		byteArray.write_int(furniture_permission);
		byteArray.write_uint16(furniture_vec.size());
		for(size_t i=0; i<furniture_vec.size();i++)
		{
			furniture_vec[i].encode(byteArray);
		}

		byteArray.write_uint16(room_tex_vec.size());
		for(size_t i=0; i<room_tex_vec.size();i++)
		{
			room_tex_vec[i].encode(byteArray);
		}

		byteArray.write_uint16(component_tex_vec.size());
		for(size_t i=0; i<component_tex_vec.size();i++)
		{
			component_tex_vec[i].encode(byteArray);
		}

		byteArray.write_uint16(component_mesh_vec.size());
		for(size_t i=0; i<component_mesh_vec.size();i++)
		{
			component_mesh_vec[i].encode(byteArray);
		}

		byteArray.write_string(welcome_words);
		byteArray.write_int(house_permission);
		byteArray.write_uint16(visit_log.size());
		for(size_t i=0; i<visit_log.size();i++)
		{
			visit_log[i].encode(byteArray);
		}

		buy_date.encode(byteArray);

		byteArray.write_int(level);
		byteArray.write_uint16(gift_box_vec.size());
		for(size_t i=0; i<gift_box_vec.size();i++)
		{
			gift_box_vec[i].encode(byteArray);
		}

		byteArray.write_uint16(magic_box_vec.size());
		for(size_t i=0; i<magic_box_vec.size();i++)
		{
			magic_box_vec[i].encode(byteArray);
		}

		byteArray.write_int(house_clean);
	}

	void decode(ByteArray &byteArray)
	{
		owner = byteArray.read_string();
		instance_id = byteArray.read_uint64();
		template_id = byteArray.read_int();
		furniture_permission = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		furniture_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			furniture_vec[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		room_tex_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			room_tex_vec[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		component_tex_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			component_tex_vec[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		component_mesh_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			component_mesh_vec[i].decode(byteArray);
		}
		}

		welcome_words = byteArray.read_string();
		house_permission = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		visit_log.resize(size);
		for(int i=0; i<size;i++)
		{
			visit_log[i].decode(byteArray);
		}
		}

		buy_date.decode(byteArray);

		level = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		gift_box_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			gift_box_vec[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		magic_box_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			magic_box_vec[i].decode(byteArray);
		}
		}

		house_clean = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_house_data);
		encode(byteArray);
	}
};

struct notify_house_clean : public INetPacket
{
	int house_clean;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(house_clean);
	}

	void decode(ByteArray &byteArray)
	{
		house_clean = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_clean);
		encode(byteArray);
	}
};

struct notify_house_data : public INetPacket
{
	house_data data;
	void encode(ByteArray &byteArray)
	{
		data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_data);
		encode(byteArray);
	}
};

struct notify_house_gift_box : public INetPacket
{
	std::vector<gift_box> gift_box_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(gift_box_vec.size());
		for(size_t i=0; i<gift_box_vec.size();i++)
		{
			gift_box_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		gift_box_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			gift_box_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_gift_box);
		encode(byteArray);
	}
};

struct req_placed_furniture : public INetPacket
{
	int grid_index;
	int x;
	int z;
	int height;
	int floor;
	int face;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(grid_index);
		byteArray.write_int(x);
		byteArray.write_int(z);
		byteArray.write_int(height);
		byteArray.write_int(floor);
		byteArray.write_int(face);
	}

	void decode(ByteArray &byteArray)
	{
		grid_index = byteArray.read_int();
		x = byteArray.read_int();
		z = byteArray.read_int();
		height = byteArray.read_int();
		floor = byteArray.read_int();
		face = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_placed_furniture);
		encode(byteArray);
	}
};

struct req_start_edit_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_edit_house);
		encode(byteArray);
	}
};

struct notify_start_edit_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_edit_house);
		encode(byteArray);
	}
};

struct req_end_edit_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_edit_house);
		encode(byteArray);
	}
};

struct notify_end_edit_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_edit_house);
		encode(byteArray);
	}
};

struct req_recovery_furniture : public INetPacket
{
	uint64 furniture_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(furniture_id);
	}

	void decode(ByteArray &byteArray)
	{
		furniture_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_recovery_furniture);
		encode(byteArray);
	}
};

struct req_replace_component_mesh : public INetPacket
{
	house_component_mesh component_mesh;
	void encode(ByteArray &byteArray)
	{
		component_mesh.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		component_mesh.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_replace_component_mesh);
		encode(byteArray);
	}
};

struct req_replace_component_tex : public INetPacket
{
	house_component_tex component_tex;
	void encode(ByteArray &byteArray)
	{
		component_tex.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		component_tex.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_replace_component_tex);
		encode(byteArray);
	}
};

struct req_replace_room_tex : public INetPacket
{
	room_tex room_tex_info;
	int goods_id;
	void encode(ByteArray &byteArray)
	{
		room_tex_info.encode(byteArray);

		byteArray.write_int(goods_id);
	}

	void decode(ByteArray &byteArray)
	{
		room_tex_info.decode(byteArray);

		goods_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_replace_room_tex);
		encode(byteArray);
	}
};

struct notify_replace_room_tex : public INetPacket
{
	room_tex room_tex_info;
	void encode(ByteArray &byteArray)
	{
		room_tex_info.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		room_tex_info.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_replace_room_tex);
		encode(byteArray);
	}
};

struct req_move_furniture : public INetPacket
{
	uint64 instance_id;
	int x;
	int z;
	int height;
	int floor;
	int face;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(x);
		byteArray.write_int(z);
		byteArray.write_int(height);
		byteArray.write_int(floor);
		byteArray.write_int(face);
	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		x = byteArray.read_int();
		z = byteArray.read_int();
		height = byteArray.read_int();
		floor = byteArray.read_int();
		face = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_move_furniture);
		encode(byteArray);
	}
};

struct notify_add_furniture : public INetPacket
{
	uint64 instance_id;
	int template_id;
	int x;
	int z;
	int height;
	int floor;
	int face;
	stime del_time;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(template_id);
		byteArray.write_int(x);
		byteArray.write_int(z);
		byteArray.write_int(height);
		byteArray.write_int(floor);
		byteArray.write_int(face);
		del_time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		template_id = byteArray.read_int();
		x = byteArray.read_int();
		z = byteArray.read_int();
		height = byteArray.read_int();
		floor = byteArray.read_int();
		face = byteArray.read_int();
		del_time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_add_furniture);
		encode(byteArray);
	}
};

struct notify_delete_furniture : public INetPacket
{
	uint64 instance_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_delete_furniture);
		encode(byteArray);
	}
};

struct req_house_basic_data : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_house_basic_data);
		encode(byteArray);
	}
};

struct notify_house_basic_data : public INetPacket
{
	house_data data;
	void encode(ByteArray &byteArray)
	{
		data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_basic_data);
		encode(byteArray);
	}
};

struct req_set_house_welcome_words : public INetPacket
{
	std::string words;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(words);
	}

	void decode(ByteArray &byteArray)
	{
		words = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_set_house_welcome_words);
		encode(byteArray);
	}
};

struct notify_set_house_welcome_words : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_set_house_welcome_words);
		encode(byteArray);
	}
};

struct req_set_house_permission : public INetPacket
{
	int house_permission;
	int furniture_permission;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(house_permission);
		byteArray.write_int(furniture_permission);
	}

	void decode(ByteArray &byteArray)
	{
		house_permission = byteArray.read_int();
		furniture_permission = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_set_house_permission);
		encode(byteArray);
	}
};

struct notify_set_house_permission : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_set_house_permission);
		encode(byteArray);
	}
};

struct req_clear_house_log : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_clear_house_log);
		encode(byteArray);
	}
};

struct notify_clear_house_log : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_clear_house_log);
		encode(byteArray);
	}
};

struct req_change_house : public INetPacket
{
	int type;
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_change_house);
		encode(byteArray);
	}
};

struct notify_change_house : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_change_house);
		encode(byteArray);
	}
};

struct req_house_list : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_house_list);
		encode(byteArray);
	}
};

struct house_plot_info : public INetPacket
{
	uint64 instance_id;
	int template_id;
	stime buy_date;
	uint64 plot_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(template_id);
		buy_date.encode(byteArray);

		byteArray.write_uint64(plot_id);
	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		template_id = byteArray.read_int();
		buy_date.decode(byteArray);

		plot_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_house_plot_info);
		encode(byteArray);
	}
};

struct notify_house_list : public INetPacket
{
	std::string account;
	std::vector<house_plot_info> house_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_uint16(house_vec.size());
		for(size_t i=0; i<house_vec.size();i++)
		{
			house_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		house_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			house_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_list);
		encode(byteArray);
	}
};

struct notify_player_property_changed : public INetPacket
{
	int player_clean;
	int player_health;
	int player_charm;
	int active_value;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(player_clean);
		byteArray.write_int(player_health);
		byteArray.write_int(player_charm);
		byteArray.write_int(active_value);
	}

	void decode(ByteArray &byteArray)
	{
		player_clean = byteArray.read_int();
		player_health = byteArray.read_int();
		player_charm = byteArray.read_int();
		active_value = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_property_changed);
		encode(byteArray);
	}
};

struct control_furni_info : public INetPacket
{
	uint64 instance_id;
	int function_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(function_id);
	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		function_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_control_furni_info);
		encode(byteArray);
	}
};

struct mutli_furni_info : public INetPacket
{
	std::string account;
	int position_index;
	uint64 instance_id;
	int status;
	int function_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(position_index);
		byteArray.write_uint64(instance_id);
		byteArray.write_int(status);
		byteArray.write_int(function_id);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		position_index = byteArray.read_int();
		instance_id = byteArray.read_uint64();
		status = byteArray.read_int();
		function_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_mutli_furni_info);
		encode(byteArray);
	}
};

struct req_use_mutli_furni : public INetPacket
{
	std::vector<control_furni_info> furni_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(furni_vec.size());
		for(size_t i=0; i<furni_vec.size();i++)
		{
			furni_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		furni_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			furni_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_use_mutli_furni);
		encode(byteArray);
	}
};

struct notify_use_mutli_furni : public INetPacket
{
	std::vector<mutli_furni_info> furni_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(furni_vec.size());
		for(size_t i=0; i<furni_vec.size();i++)
		{
			furni_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		furni_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			furni_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_use_mutli_furni);
		encode(byteArray);
	}
};

struct notify_use_furniture_result : public INetPacket
{
	std::string account;
	int position_index;
	uint64 instance_id;
	int status;
	int function_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(position_index);
		byteArray.write_uint64(instance_id);
		byteArray.write_int(status);
		byteArray.write_int(function_id);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		position_index = byteArray.read_int();
		instance_id = byteArray.read_uint64();
		status = byteArray.read_int();
		function_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_use_furniture_result);
		encode(byteArray);
	}
};

struct req_stop_using_furniture : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_stop_using_furniture);
		encode(byteArray);
	}
};

struct notify_stop_using_furniture_result : public INetPacket
{
	std::string account;
	int position_index;
	uint64 instance_id;
	int function_id;
	int pre_status;
	int status;
	point pos;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(position_index);
		byteArray.write_uint64(instance_id);
		byteArray.write_int(function_id);
		byteArray.write_int(pre_status);
		byteArray.write_int(status);
		pos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		position_index = byteArray.read_int();
		instance_id = byteArray.read_uint64();
		function_id = byteArray.read_int();
		pre_status = byteArray.read_int();
		status = byteArray.read_int();
		pos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_stop_using_furniture_result);
		encode(byteArray);
	}
};

struct notify_package : public INetPacket
{
	std::string account;
	int type;
	std::vector<pack_grid> grid_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(type);
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		type = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_package);
		encode(byteArray);
	}
};

struct req_swap_item : public INetPacket
{
	int type;
	int index1;
	int index2;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_int(index1);
		byteArray.write_int(index2);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		index1 = byteArray.read_int();
		index2 = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_swap_item);
		encode(byteArray);
	}
};

struct req_use_item : public INetPacket
{
	int index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(index);
	}

	void decode(ByteArray &byteArray)
	{
		index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_use_item);
		encode(byteArray);
	}
};

struct req_move_item : public INetPacket
{
	int pack1_type;
	int index1;
	int pack2_type;
	int index2;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(pack1_type);
		byteArray.write_int(index1);
		byteArray.write_int(pack2_type);
		byteArray.write_int(index2);
	}

	void decode(ByteArray &byteArray)
	{
		pack1_type = byteArray.read_int();
		index1 = byteArray.read_int();
		pack2_type = byteArray.read_int();
		index2 = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_move_item);
		encode(byteArray);
	}
};

struct req_delete_item : public INetPacket
{
	int grid_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(grid_index);
	}

	void decode(ByteArray &byteArray)
	{
		grid_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_delete_item);
		encode(byteArray);
	}
};

struct req_split_item : public INetPacket
{
	int type;
	int src_gindex;
	int target_gindex;
	int count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_int(src_gindex);
		byteArray.write_int(target_gindex);
		byteArray.write_int(count);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		src_gindex = byteArray.read_int();
		target_gindex = byteArray.read_int();
		count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_split_item);
		encode(byteArray);
	}
};

struct req_resize_player_pack : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_resize_player_pack);
		encode(byteArray);
	}
};

struct req_lock_bag_item : public INetPacket
{
	uint64 item_inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(item_inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		item_inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_lock_bag_item);
		encode(byteArray);
	}
};

struct req_unlock_bag_item : public INetPacket
{
	uint64 item_inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(item_inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		item_inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_unlock_bag_item);
		encode(byteArray);
	}
};

struct req_move_money : public INetPacket
{
	int pack1_type;
	int pack2_type;
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(pack1_type);
		byteArray.write_int(pack2_type);
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		pack1_type = byteArray.read_int();
		pack2_type = byteArray.read_int();
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_move_money);
		encode(byteArray);
	}
};

struct req_switch_item : public INetPacket
{
	int pack1_type;
	uint64 instance_id;
	int pack2_type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(pack1_type);
		byteArray.write_uint64(instance_id);
		byteArray.write_int(pack2_type);
	}

	void decode(ByteArray &byteArray)
	{
		pack1_type = byteArray.read_int();
		instance_id = byteArray.read_uint64();
		pack2_type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_switch_item);
		encode(byteArray);
	}
};

struct notify_sys_msg : public INetPacket
{
	int code;
	std::vector<std::string> params;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(code);
		byteArray.write_uint16(params.size());
		for(size_t i=0; i<params.size();i++)
		{
			byteArray.write_string(params[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		code = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		params.reserve(size);
		for(int i=0; i<size;i++)
		{
			params.push_back(byteArray.read_string());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_sys_msg);
		encode(byteArray);
	}
};

struct notify_sys_broadcast : public INetPacket
{
	uint64 id;
	int type;
	std::string content;
	int play_times;
	int priority;
	int show_seconds;
	stime start_time;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_int(type);
		byteArray.write_string(content);
		byteArray.write_int(play_times);
		byteArray.write_int(priority);
		byteArray.write_int(show_seconds);
		start_time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		type = byteArray.read_int();
		content = byteArray.read_string();
		play_times = byteArray.read_int();
		priority = byteArray.read_int();
		show_seconds = byteArray.read_int();
		start_time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_sys_broadcast);
		encode(byteArray);
	}
};

struct req_fixed_broadcast : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_fixed_broadcast);
		encode(byteArray);
	}
};

struct notify_del_broadcast : public INetPacket
{
	int type;
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_del_broadcast);
		encode(byteArray);
	}
};

struct req_gm_command : public INetPacket
{
	int type;
	std::vector<std::string> params;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_uint16(params.size());
		for(size_t i=0; i<params.size();i++)
		{
			byteArray.write_string(params[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		params.reserve(size);
		for(int i=0; i<size;i++)
		{
			params.push_back(byteArray.read_string());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_gm_command);
		encode(byteArray);
	}
};

struct notify_npc_list : public INetPacket
{
	std::vector<npc_map_mapping_info> npc_map_mapping_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(npc_map_mapping_list.size());
		for(size_t i=0; i<npc_map_mapping_list.size();i++)
		{
			npc_map_mapping_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		npc_map_mapping_list.resize(size);
		for(int i=0; i<size;i++)
		{
			npc_map_mapping_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_npc_list);
		encode(byteArray);
	}
};

struct notify_npc_close_dialog : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_npc_close_dialog);
		encode(byteArray);
	}
};

struct notify_npc_show_dialog_by_option : public INetPacket
{
	int template_id;
	std::vector<npc_command> command_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(template_id);
		byteArray.write_uint16(command_list.size());
		for(size_t i=0; i<command_list.size();i++)
		{
			command_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		template_id = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		command_list.resize(size);
		for(int i=0; i<size;i++)
		{
			command_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_npc_show_dialog_by_option);
		encode(byteArray);
	}
};

struct notify_npc_show_dialog_by_message : public INetPacket
{
	int template_id;
	uint64 unique_id;
	int page_index;
	int command_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(template_id);
		byteArray.write_uint64(unique_id);
		byteArray.write_int(page_index);
		byteArray.write_int(command_index);
	}

	void decode(ByteArray &byteArray)
	{
		template_id = byteArray.read_int();
		unique_id = byteArray.read_uint64();
		page_index = byteArray.read_int();
		command_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_npc_show_dialog_by_message);
		encode(byteArray);
	}
};

struct notify_npc_show_dialog_by_item : public INetPacket
{
	int template_id;
	std::vector<npc_command> item_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(template_id);
		byteArray.write_uint16(item_list.size());
		for(size_t i=0; i<item_list.size();i++)
		{
			item_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		template_id = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		item_list.resize(size);
		for(int i=0; i<size;i++)
		{
			item_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_npc_show_dialog_by_item);
		encode(byteArray);
	}
};

struct req_npc_command : public INetPacket
{
	std::string npc_key;
	uint64 unique_id;
	int page_index;
	int command_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(npc_key);
		byteArray.write_uint64(unique_id);
		byteArray.write_int(page_index);
		byteArray.write_int(command_index);
	}

	void decode(ByteArray &byteArray)
	{
		npc_key = byteArray.read_string();
		unique_id = byteArray.read_uint64();
		page_index = byteArray.read_int();
		command_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_npc_command);
		encode(byteArray);
	}
};

struct req_player_basic_data : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_player_basic_data);
		encode(byteArray);
	}
};

struct notify_player_basic_data : public INetPacket
{
	player_basic_data basic_data;
	player_property property;
	void encode(ByteArray &byteArray)
	{
		basic_data.encode(byteArray);

		property.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		basic_data.decode(byteArray);

		property.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_basic_data);
		encode(byteArray);
	}
};

struct req_start_body_action : public INetPacket
{
	std::string target_account;
	std::string action;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(target_account);
		byteArray.write_string(action);
	}

	void decode(ByteArray &byteArray)
	{
		target_account = byteArray.read_string();
		action = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_body_action);
		encode(byteArray);
	}
};

struct notify_start_body_action : public INetPacket
{
	std::string player_account;
	std::string target_account;
	std::string action;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(player_account);
		byteArray.write_string(target_account);
		byteArray.write_string(action);
	}

	void decode(ByteArray &byteArray)
	{
		player_account = byteArray.read_string();
		target_account = byteArray.read_string();
		action = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_body_action);
		encode(byteArray);
	}
};

struct req_play_animation : public INetPacket
{
	std::string target_account;
	int loop;
	std::string ani;
	std::string action;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(target_account);
		byteArray.write_int(loop);
		byteArray.write_string(ani);
		byteArray.write_string(action);
	}

	void decode(ByteArray &byteArray)
	{
		target_account = byteArray.read_string();
		loop = byteArray.read_int();
		ani = byteArray.read_string();
		action = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_play_animation);
		encode(byteArray);
	}
};

struct notify_play_animation : public INetPacket
{
	std::string player_account;
	std::string target_account;
	int loop;
	std::string ani;
	std::string action;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(player_account);
		byteArray.write_string(target_account);
		byteArray.write_int(loop);
		byteArray.write_string(ani);
		byteArray.write_string(action);
	}

	void decode(ByteArray &byteArray)
	{
		player_account = byteArray.read_string();
		target_account = byteArray.read_string();
		loop = byteArray.read_int();
		ani = byteArray.read_string();
		action = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_play_animation);
		encode(byteArray);
	}
};

struct req_end_body_action : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_body_action);
		encode(byteArray);
	}
};

struct notify_end_body_action : public INetPacket
{
	std::string player_account;
	std::string target_account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(player_account);
		byteArray.write_string(target_account);
	}

	void decode(ByteArray &byteArray)
	{
		player_account = byteArray.read_string();
		target_account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_body_action);
		encode(byteArray);
	}
};

struct req_sync_body_state : public INetPacket
{
	int body_state;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(body_state);
	}

	void decode(ByteArray &byteArray)
	{
		body_state = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sync_body_state);
		encode(byteArray);
	}
};

struct notify_other_body_state : public INetPacket
{
	std::string other_account;
	int body_state;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(other_account);
		byteArray.write_int(body_state);
	}

	void decode(ByteArray &byteArray)
	{
		other_account = byteArray.read_string();
		body_state = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_body_state);
		encode(byteArray);
	}
};

struct req_start_change_looks : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_change_looks);
		encode(byteArray);
	}
};

struct notify_start_change_looks : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_change_looks);
		encode(byteArray);
	}
};

struct req_cancel_change_looks : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_cancel_change_looks);
		encode(byteArray);
	}
};

struct req_end_change_looks : public INetPacket
{
	int new_hair;
	int new_hair_color;
	int new_face;
	int new_skin_color;
	int new_beard;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(new_hair);
		byteArray.write_int(new_hair_color);
		byteArray.write_int(new_face);
		byteArray.write_int(new_skin_color);
		byteArray.write_int(new_beard);
	}

	void decode(ByteArray &byteArray)
	{
		new_hair = byteArray.read_int();
		new_hair_color = byteArray.read_int();
		new_face = byteArray.read_int();
		new_skin_color = byteArray.read_int();
		new_beard = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_change_looks);
		encode(byteArray);
	}
};

struct notify_change_looks : public INetPacket
{
	std::string account;
	int new_hair;
	int new_hair_color;
	int new_face;
	int new_skin_color;
	int new_beard;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(new_hair);
		byteArray.write_int(new_hair_color);
		byteArray.write_int(new_face);
		byteArray.write_int(new_skin_color);
		byteArray.write_int(new_beard);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		new_hair = byteArray.read_int();
		new_hair_color = byteArray.read_int();
		new_face = byteArray.read_int();
		new_skin_color = byteArray.read_int();
		new_beard = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_change_looks);
		encode(byteArray);
	}
};

struct notify_end_change_looks : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_change_looks);
		encode(byteArray);
	}
};

struct notify_around_change_dress : public INetPacket
{
	std::string account;
	int pos;
	int template_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(pos);
		byteArray.write_int(template_id);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		pos = byteArray.read_int();
		template_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_around_change_dress);
		encode(byteArray);
	}
};

struct req_invite_someone : public INetPacket
{
	std::string invitee;
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(invitee);
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		invitee = byteArray.read_string();
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_invite_someone);
		encode(byteArray);
	}
};

struct notify_invitation : public INetPacket
{
	std::string invitor;
	std::string invitor_name;
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(invitor);
		byteArray.write_string(invitor_name);
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		invitor = byteArray.read_string();
		invitor_name = byteArray.read_string();
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_invitation);
		encode(byteArray);
	}
};

struct req_agree_invitation : public INetPacket
{
	std::string invitor;
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(invitor);
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		invitor = byteArray.read_string();
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_agree_invitation);
		encode(byteArray);
	}
};

struct req_disagree_invitation : public INetPacket
{
	std::string invitor;
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(invitor);
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		invitor = byteArray.read_string();
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_disagree_invitation);
		encode(byteArray);
	}
};

struct notify_cancel_invitation : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_cancel_invitation);
		encode(byteArray);
	}
};

struct notify_start_trade : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_trade);
		encode(byteArray);
	}
};

struct req_update_trade_money : public INetPacket
{
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_update_trade_money);
		encode(byteArray);
	}
};

struct notify_update_trade_money : public INetPacket
{
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_update_trade_money);
		encode(byteArray);
	}
};

struct notify_other_update_trade_money : public INetPacket
{
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_update_trade_money);
		encode(byteArray);
	}
};

struct req_put_trade_item : public INetPacket
{
	int packet_index;
	int trade_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(packet_index);
		byteArray.write_int(trade_index);
	}

	void decode(ByteArray &byteArray)
	{
		packet_index = byteArray.read_int();
		trade_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_put_trade_item);
		encode(byteArray);
	}
};

struct notify_put_trade_item : public INetPacket
{
	int trade_index;
	item item_data;
	int item_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(trade_index);
		item_data.encode(byteArray);

		byteArray.write_int(item_count);
	}

	void decode(ByteArray &byteArray)
	{
		trade_index = byteArray.read_int();
		item_data.decode(byteArray);

		item_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_put_trade_item);
		encode(byteArray);
	}
};

struct notify_other_put_trade_item : public INetPacket
{
	int trade_index;
	item item_data;
	int item_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(trade_index);
		item_data.encode(byteArray);

		byteArray.write_int(item_count);
	}

	void decode(ByteArray &byteArray)
	{
		trade_index = byteArray.read_int();
		item_data.decode(byteArray);

		item_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_put_trade_item);
		encode(byteArray);
	}
};

struct req_unput_trade_item : public INetPacket
{
	int trade_index;
	int packet_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(trade_index);
		byteArray.write_int(packet_index);
	}

	void decode(ByteArray &byteArray)
	{
		trade_index = byteArray.read_int();
		packet_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_unput_trade_item);
		encode(byteArray);
	}
};

struct notify_unput_trade_item : public INetPacket
{
	int trade_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(trade_index);
	}

	void decode(ByteArray &byteArray)
	{
		trade_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_unput_trade_item);
		encode(byteArray);
	}
};

struct notify_other_unput_trade_item : public INetPacket
{
	int trade_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(trade_index);
	}

	void decode(ByteArray &byteArray)
	{
		trade_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_unput_trade_item);
		encode(byteArray);
	}
};

struct req_swap_trade_item : public INetPacket
{
	int index1;
	int index2;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(index1);
		byteArray.write_int(index2);
	}

	void decode(ByteArray &byteArray)
	{
		index1 = byteArray.read_int();
		index2 = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_swap_trade_item);
		encode(byteArray);
	}
};

struct notify_swap_trade_item : public INetPacket
{
	int index1;
	int index2;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(index1);
		byteArray.write_int(index2);
	}

	void decode(ByteArray &byteArray)
	{
		index1 = byteArray.read_int();
		index2 = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_swap_trade_item);
		encode(byteArray);
	}
};

struct notify_other_swap_trade_item : public INetPacket
{
	int index1;
	int index2;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(index1);
		byteArray.write_int(index2);
	}

	void decode(ByteArray &byteArray)
	{
		index1 = byteArray.read_int();
		index2 = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_swap_trade_item);
		encode(byteArray);
	}
};

struct req_sure_trade : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sure_trade);
		encode(byteArray);
	}
};

struct notify_sure_trade : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_sure_trade);
		encode(byteArray);
	}
};

struct notify_other_sure_trade : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_other_sure_trade);
		encode(byteArray);
	}
};

struct req_cancel_trade : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_cancel_trade);
		encode(byteArray);
	}
};

struct notify_cancel_trade : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_cancel_trade);
		encode(byteArray);
	}
};

struct notify_trade_success : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_trade_success);
		encode(byteArray);
	}
};

struct furniture_goods_data : public INetPacket
{
	int goods_id;
	int x;
	int z;
	int height;
	int floor;
	int face;
	int room_id;
	int type;
	std::string tex;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(goods_id);
		byteArray.write_int(x);
		byteArray.write_int(z);
		byteArray.write_int(height);
		byteArray.write_int(floor);
		byteArray.write_int(face);
		byteArray.write_int(room_id);
		byteArray.write_int(type);
		byteArray.write_string(tex);
	}

	void decode(ByteArray &byteArray)
	{
		goods_id = byteArray.read_int();
		x = byteArray.read_int();
		z = byteArray.read_int();
		height = byteArray.read_int();
		floor = byteArray.read_int();
		face = byteArray.read_int();
		room_id = byteArray.read_int();
		type = byteArray.read_int();
		tex = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_furniture_goods_data);
		encode(byteArray);
	}
};

struct req_buy_sys_shop_goods : public INetPacket
{
	int goods_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(goods_id);
	}

	void decode(ByteArray &byteArray)
	{
		goods_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_sys_shop_goods);
		encode(byteArray);
	}
};

struct req_buy_sys_shop_goods_list : public INetPacket
{
	std::vector<furniture_goods_data> goods_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(goods_list.size());
		for(size_t i=0; i<goods_list.size();i++)
		{
			goods_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		goods_list.resize(size);
		for(int i=0; i<size;i++)
		{
			goods_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_sys_shop_goods_list);
		encode(byteArray);
	}
};

struct notify_buy_furniture_list : public INetPacket
{
	std::vector<house_furniture> furnitures;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(furnitures.size());
		for(size_t i=0; i<furnitures.size();i++)
		{
			furnitures[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		furnitures.resize(size);
		for(int i=0; i<size;i++)
		{
			furnitures[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_buy_furniture_list);
		encode(byteArray);
	}
};

struct player_coin : public INetPacket
{
	std::string account;
	int gamecoin;
	int eqcoin;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(gamecoin);
		byteArray.write_int(eqcoin);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		gamecoin = byteArray.read_int();
		eqcoin = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_coin);
		encode(byteArray);
	}
};

struct notify_update_coin : public INetPacket
{
	player_coin data;
	void encode(ByteArray &byteArray)
	{
		data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_update_coin);
		encode(byteArray);
	}
};

struct req_buy_npc_shop_goods : public INetPacket
{
	int goods_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(goods_id);
	}

	void decode(ByteArray &byteArray)
	{
		goods_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_npc_shop_goods);
		encode(byteArray);
	}
};

struct req_sell_goods : public INetPacket
{
	int grid_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(grid_index);
	}

	void decode(ByteArray &byteArray)
	{
		grid_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sell_goods);
		encode(byteArray);
	}
};

struct notify_open_shop : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_open_shop);
		encode(byteArray);
	}
};

struct farm_pet_food : public INetPacket
{
	stime period_date;
	void encode(ByteArray &byteArray)
	{
		period_date.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		period_date.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_pet_food);
		encode(byteArray);
	}
};

struct farm_log_message : public INetPacket
{
	int log_type;
	std::string account;
	stime log_date;
	std::string user_name;
	int crop_id;
	int crop_quantity;
	int pet_id;
	int bite_coin;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(log_type);
		byteArray.write_string(account);
		log_date.encode(byteArray);

		byteArray.write_string(user_name);
		byteArray.write_int(crop_id);
		byteArray.write_int(crop_quantity);
		byteArray.write_int(pet_id);
		byteArray.write_int(bite_coin);
	}

	void decode(ByteArray &byteArray)
	{
		log_type = byteArray.read_int();
		account = byteArray.read_string();
		log_date.decode(byteArray);

		user_name = byteArray.read_string();
		crop_id = byteArray.read_int();
		crop_quantity = byteArray.read_int();
		pet_id = byteArray.read_int();
		bite_coin = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_log_message);
		encode(byteArray);
	}
};

struct farm_log_gain : public INetPacket
{
	int crop_id;
	int crop_quantity;
	int steal_quantity;
	int gain_quantity;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(crop_id);
		byteArray.write_int(crop_quantity);
		byteArray.write_int(steal_quantity);
		byteArray.write_int(gain_quantity);
	}

	void decode(ByteArray &byteArray)
	{
		crop_id = byteArray.read_int();
		crop_quantity = byteArray.read_int();
		steal_quantity = byteArray.read_int();
		gain_quantity = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_log_gain);
		encode(byteArray);
	}
};

struct farm_log_consumption : public INetPacket
{
	int type;
	stime log_date;
	int crop_id;
	int crop_quantity;
	int use_coin;
	int leave_coin;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		log_date.encode(byteArray);

		byteArray.write_int(crop_id);
		byteArray.write_int(crop_quantity);
		byteArray.write_int(use_coin);
		byteArray.write_int(leave_coin);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		log_date.decode(byteArray);

		crop_id = byteArray.read_int();
		crop_quantity = byteArray.read_int();
		use_coin = byteArray.read_int();
		leave_coin = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_log_consumption);
		encode(byteArray);
	}
};

struct farm_comment : public INetPacket
{
	int type;
	std::string account;
	stime comment_date;
	std::string username;
	std::string content;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_string(account);
		comment_date.encode(byteArray);

		byteArray.write_string(username);
		byteArray.write_string(content);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		account = byteArray.read_string();
		comment_date.decode(byteArray);

		username = byteArray.read_string();
		content = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_comment);
		encode(byteArray);
	}
};

struct farm_decoration : public INetPacket
{
	item background;
	item house;
	item doghouse;
	item dog;
	item signpost;
	item fence;
	item scarecrow;
	item billboard;
	item billboard_content;
	void encode(ByteArray &byteArray)
	{
		background.encode(byteArray);

		house.encode(byteArray);

		doghouse.encode(byteArray);

		dog.encode(byteArray);

		signpost.encode(byteArray);

		fence.encode(byteArray);

		scarecrow.encode(byteArray);

		billboard.encode(byteArray);

		billboard_content.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		background.decode(byteArray);

		house.decode(byteArray);

		doghouse.decode(byteArray);

		dog.decode(byteArray);

		signpost.decode(byteArray);

		fence.decode(byteArray);

		scarecrow.decode(byteArray);

		billboard.decode(byteArray);

		billboard_content.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_decoration);
		encode(byteArray);
	}
};

struct farm_crop_data : public INetPacket
{
	int tempid;
	int health;
	std::vector<int> curr_damage;
	stime start_time;
	int pick_num;
	int remain;
	int output;
	int stage;
	int land_type;
	int picked;
	std::vector<int> stage_time;
	int aberrance_fruit;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(tempid);
		byteArray.write_int(health);
		byteArray.write_uint16(curr_damage.size());
		for(size_t i=0; i<curr_damage.size();i++)
		{
			byteArray.write_int(curr_damage[i]);
		}

		start_time.encode(byteArray);

		byteArray.write_int(pick_num);
		byteArray.write_int(remain);
		byteArray.write_int(output);
		byteArray.write_int(stage);
		byteArray.write_int(land_type);
		byteArray.write_int(picked);
		byteArray.write_uint16(stage_time.size());
		for(size_t i=0; i<stage_time.size();i++)
		{
			byteArray.write_int(stage_time[i]);
		}

		byteArray.write_int(aberrance_fruit);
	}

	void decode(ByteArray &byteArray)
	{
		tempid = byteArray.read_int();
		health = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		curr_damage.reserve(size);
		for(int i=0; i<size;i++)
		{
			curr_damage.push_back(byteArray.read_int());
		}
		}

		start_time.decode(byteArray);

		pick_num = byteArray.read_int();
		remain = byteArray.read_int();
		output = byteArray.read_int();
		stage = byteArray.read_int();
		land_type = byteArray.read_int();
		picked = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		stage_time.reserve(size);
		for(int i=0; i<size;i++)
		{
			stage_time.push_back(byteArray.read_int());
		}
		}

		aberrance_fruit = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_crop_data);
		encode(byteArray);
	}
};

struct player_farm_data : public INetPacket
{
	std::string account;
	int exp;
	farm_decoration decoration;
	farm_pet_food pet_food;
	std::vector<farm_crop_data> crops;
	int level;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(exp);
		decoration.encode(byteArray);

		pet_food.encode(byteArray);

		byteArray.write_uint16(crops.size());
		for(size_t i=0; i<crops.size();i++)
		{
			crops[i].encode(byteArray);
		}

		byteArray.write_int(level);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		exp = byteArray.read_int();
		decoration.decode(byteArray);

		pet_food.decode(byteArray);

		{
		UINT16 size = byteArray.read_uint16();
		crops.resize(size);
		for(int i=0; i<size;i++)
		{
			crops[i].decode(byteArray);
		}
		}

		level = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_farm_data);
		encode(byteArray);
	}
};

struct farm_exp_data : public INetPacket
{
	int plot_index;
	int add_exp;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(plot_index);
		byteArray.write_int(add_exp);
	}

	void decode(ByteArray &byteArray)
	{
		plot_index = byteArray.read_int();
		add_exp = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_exp_data);
		encode(byteArray);
	}
};

struct farm_hint_data : public INetPacket
{
	int plot_index;
	int hint_id;
	int param1;
	int param2;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(plot_index);
		byteArray.write_int(hint_id);
		byteArray.write_int(param1);
		byteArray.write_int(param2);
	}

	void decode(ByteArray &byteArray)
	{
		plot_index = byteArray.read_int();
		hint_id = byteArray.read_int();
		param1 = byteArray.read_int();
		param2 = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_hint_data);
		encode(byteArray);
	}
};

struct farm_pick_all_result : public INetPacket
{
	player_farm_data farm_data;
	std::vector<farm_exp_data> exp_data;
	std::vector<farm_hint_data> hint;
	void encode(ByteArray &byteArray)
	{
		farm_data.encode(byteArray);

		byteArray.write_uint16(exp_data.size());
		for(size_t i=0; i<exp_data.size();i++)
		{
			exp_data[i].encode(byteArray);
		}

		byteArray.write_uint16(hint.size());
		for(size_t i=0; i<hint.size();i++)
		{
			hint[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		farm_data.decode(byteArray);

		{
		UINT16 size = byteArray.read_uint16();
		exp_data.resize(size);
		for(int i=0; i<size;i++)
		{
			exp_data[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		hint.resize(size);
		for(int i=0; i<size;i++)
		{
			hint[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_pick_all_result);
		encode(byteArray);
	}
};

struct player_farm_log : public INetPacket
{
	std::string account;
	std::vector<farm_log_message> message_log;
	std::vector<farm_log_gain> gain_log;
	std::vector<farm_log_consumption> consumption_log;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_uint16(message_log.size());
		for(size_t i=0; i<message_log.size();i++)
		{
			message_log[i].encode(byteArray);
		}

		byteArray.write_uint16(gain_log.size());
		for(size_t i=0; i<gain_log.size();i++)
		{
			gain_log[i].encode(byteArray);
		}

		byteArray.write_uint16(consumption_log.size());
		for(size_t i=0; i<consumption_log.size();i++)
		{
			consumption_log[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		message_log.resize(size);
		for(int i=0; i<size;i++)
		{
			message_log[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		gain_log.resize(size);
		for(int i=0; i<size;i++)
		{
			gain_log[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		consumption_log.resize(size);
		for(int i=0; i<size;i++)
		{
			consumption_log[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_farm_log);
		encode(byteArray);
	}
};

struct player_farm_comment : public INetPacket
{
	std::string account;
	std::vector<farm_comment> comment;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_uint16(comment.size());
		for(size_t i=0; i<comment.size();i++)
		{
			comment[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		comment.resize(size);
		for(int i=0; i<size;i++)
		{
			comment[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_player_farm_comment);
		encode(byteArray);
	}
};

struct farm_status : public INetPacket
{
	std::string account;
	std::string name;
	int exp;
	int status;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(name);
		byteArray.write_int(exp);
		byteArray.write_int(status);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		name = byteArray.read_string();
		exp = byteArray.read_int();
		status = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_status);
		encode(byteArray);
	}
};

struct req_enter_farm : public INetPacket
{
	std::string owner;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(owner);
	}

	void decode(ByteArray &byteArray)
	{
		owner = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_enter_farm);
		encode(byteArray);
	}
};

struct notify_farm_data : public INetPacket
{
	player_farm_data data;
	void encode(ByteArray &byteArray)
	{
		data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_data);
		encode(byteArray);
	}
};

struct req_leave_farm : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_leave_farm);
		encode(byteArray);
	}
};

struct notify_leave_farm : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_leave_farm);
		encode(byteArray);
	}
};

struct req_assart_plot : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_assart_plot);
		encode(byteArray);
	}
};

struct req_sow : public INetPacket
{
	int plot_index;
	uint64 item_inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(plot_index);
		byteArray.write_uint64(item_inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		plot_index = byteArray.read_int();
		item_inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sow);
		encode(byteArray);
	}
};

struct req_pick_crop : public INetPacket
{
	std::string account;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_pick_crop);
		encode(byteArray);
	}
};

struct req_pick_all_crops : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_pick_all_crops);
		encode(byteArray);
	}
};

struct req_get_friend_farm_state : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_friend_farm_state);
		encode(byteArray);
	}
};

struct notify_friend_farm_status : public INetPacket
{
	std::vector<farm_status> data;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(data.size());
		for(size_t i=0; i<data.size();i++)
		{
			data[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		data.resize(size);
		for(int i=0; i<size;i++)
		{
			data[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_friend_farm_status);
		encode(byteArray);
	}
};

struct req_weed : public INetPacket
{
	std::string account;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_weed);
		encode(byteArray);
	}
};

struct req_worm : public INetPacket
{
	std::string account;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_worm);
		encode(byteArray);
	}
};

struct req_water : public INetPacket
{
	std::string account;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_water);
		encode(byteArray);
	}
};

struct notify_pet_bite : public INetPacket
{
	int hurt;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(hurt);
	}

	void decode(ByteArray &byteArray)
	{
		hurt = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_pet_bite);
		encode(byteArray);
	}
};

struct req_farm_log_list : public INetPacket
{
	int type;
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_farm_log_list);
		encode(byteArray);
	}
};

struct notify_farm_log_list : public INetPacket
{
	player_farm_log log;
	void encode(ByteArray &byteArray)
	{
		log.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		log.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_log_list);
		encode(byteArray);
	}
};

struct req_clear_farm_log : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_clear_farm_log);
		encode(byteArray);
	}
};

struct req_buy_farm_shop_goods : public INetPacket
{
	int type;
	int item_id;
	int count;
	int pay_type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_int(item_id);
		byteArray.write_int(count);
		byteArray.write_int(pay_type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		item_id = byteArray.read_int();
		count = byteArray.read_int();
		pay_type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_farm_shop_goods);
		encode(byteArray);
	}
};

struct req_sell_farm_shop_goods : public INetPacket
{
	uint64 instance_id;
	int count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(instance_id);
		byteArray.write_int(count);
	}

	void decode(ByteArray &byteArray)
	{
		instance_id = byteArray.read_uint64();
		count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sell_farm_shop_goods);
		encode(byteArray);
	}
};

struct req_sell_farm_shop_all_goods : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sell_farm_shop_all_goods);
		encode(byteArray);
	}
};

struct notify_farm_info : public INetPacket
{
	std::string account;
	int plot_index;
	int exp;
	int level;
	int add_exp;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
		byteArray.write_int(exp);
		byteArray.write_int(level);
		byteArray.write_int(add_exp);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
		exp = byteArray.read_int();
		level = byteArray.read_int();
		add_exp = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_info);
		encode(byteArray);
	}
};

struct req_farm_comment : public INetPacket
{
	std::string account;
	std::string comment;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(comment);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		comment = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_farm_comment);
		encode(byteArray);
	}
};

struct notify_farm_comment : public INetPacket
{
	farm_comment comment;
	void encode(ByteArray &byteArray)
	{
		comment.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		comment.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_comment);
		encode(byteArray);
	}
};

struct req_reply_farm_comment : public INetPacket
{
	std::string account;
	std::string comment;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(comment);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		comment = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_reply_farm_comment);
		encode(byteArray);
	}
};

struct notify_reply_farm_comment : public INetPacket
{
	farm_comment comment;
	void encode(ByteArray &byteArray)
	{
		comment.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		comment.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_reply_farm_comment);
		encode(byteArray);
	}
};

struct req_farm_comment_list : public INetPacket
{
	std::string account;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_farm_comment_list);
		encode(byteArray);
	}
};

struct notify_farm_comment_list : public INetPacket
{
	player_farm_comment farm_comment;
	void encode(ByteArray &byteArray)
	{
		farm_comment.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		farm_comment.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_comment_list);
		encode(byteArray);
	}
};

struct req_clear_farm_comment : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_clear_farm_comment);
		encode(byteArray);
	}
};

struct notify_clear_farm_comment : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_clear_farm_comment);
		encode(byteArray);
	}
};

struct req_remove_decoration : public INetPacket
{
	int template_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(template_id);
	}

	void decode(ByteArray &byteArray)
	{
		template_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_remove_decoration);
		encode(byteArray);
	}
};

struct req_change_decoration : public INetPacket
{
	item decoration_item;
	void encode(ByteArray &byteArray)
	{
		decoration_item.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		decoration_item.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_change_decoration);
		encode(byteArray);
	}
};

struct req_put_grass : public INetPacket
{
	std::string account;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_put_grass);
		encode(byteArray);
	}
};

struct req_put_pest : public INetPacket
{
	std::string account;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_put_pest);
		encode(byteArray);
	}
};

struct farm_setting : public INetPacket
{
	std::string account;
	std::string water;
	std::string weed;
	std::string worm;
	std::string put_grass;
	std::string put_pest;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_string(water);
		byteArray.write_string(weed);
		byteArray.write_string(worm);
		byteArray.write_string(put_grass);
		byteArray.write_string(put_pest);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		water = byteArray.read_string();
		weed = byteArray.read_string();
		worm = byteArray.read_string();
		put_grass = byteArray.read_string();
		put_pest = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_farm_setting);
		encode(byteArray);
	}
};

struct req_farm_setting : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_farm_setting);
		encode(byteArray);
	}
};

struct req_set_farm_setting : public INetPacket
{
	farm_setting setting;
	void encode(ByteArray &byteArray)
	{
		setting.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		setting.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_set_farm_setting);
		encode(byteArray);
	}
};

struct notify_set_farm_setting : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_set_farm_setting);
		encode(byteArray);
	}
};

struct req_reset_farm_setting : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_reset_farm_setting);
		encode(byteArray);
	}
};

struct notify_reset_farm_setting : public INetPacket
{
	farm_setting setting;
	void encode(ByteArray &byteArray)
	{
		setting.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		setting.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_reset_farm_setting);
		encode(byteArray);
	}
};

struct notify_farm_setting : public INetPacket
{
	farm_setting setting;
	void encode(ByteArray &byteArray)
	{
		setting.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		setting.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_setting);
		encode(byteArray);
	}
};

struct req_lock_depot_item : public INetPacket
{
	uint64 item_inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(item_inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		item_inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_lock_depot_item);
		encode(byteArray);
	}
};

struct req_unlock_depot_item : public INetPacket
{
	uint64 item_inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(item_inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		item_inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_unlock_depot_item);
		encode(byteArray);
	}
};

struct req_hoeing : public INetPacket
{
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_hoeing);
		encode(byteArray);
	}
};

struct req_use_farm_item : public INetPacket
{
	uint64 item_inst_id;
	int plot_index;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(item_inst_id);
		byteArray.write_int(plot_index);
	}

	void decode(ByteArray &byteArray)
	{
		item_inst_id = byteArray.read_uint64();
		plot_index = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_use_farm_item);
		encode(byteArray);
	}
};

struct req_can_assart_plot : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_can_assart_plot);
		encode(byteArray);
	}
};

struct req_can_upgrade_land : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_can_upgrade_land);
		encode(byteArray);
	}
};

struct req_upgrade_land : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_upgrade_land);
		encode(byteArray);
	}
};

struct notify_farm_money : public INetPacket
{
	std::string account;
	int add_money;
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(add_money);
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		add_money = byteArray.read_int();
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_money);
		encode(byteArray);
	}
};

struct notify_farm_awards : public INetPacket
{
	int type;
	int money;
	std::vector<int> itemlist;
	std::vector<int> countlist;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_int(money);
		byteArray.write_uint16(itemlist.size());
		for(size_t i=0; i<itemlist.size();i++)
		{
			byteArray.write_int(itemlist[i]);
		}

		byteArray.write_uint16(countlist.size());
		for(size_t i=0; i<countlist.size();i++)
		{
			byteArray.write_int(countlist[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		money = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		itemlist.reserve(size);
		for(int i=0; i<size;i++)
		{
			itemlist.push_back(byteArray.read_int());
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		countlist.reserve(size);
		for(int i=0; i<size;i++)
		{
			countlist.push_back(byteArray.read_int());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_awards);
		encode(byteArray);
	}
};

struct notify_farm_eq_coin : public INetPacket
{
	int eq_coin;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(eq_coin);
	}

	void decode(ByteArray &byteArray)
	{
		eq_coin = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_eq_coin);
		encode(byteArray);
	}
};

struct req_task_list : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_task_list);
		encode(byteArray);
	}
};

struct notify_task_list : public INetPacket
{
	std::vector<player_task> task_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(task_list.size());
		for(size_t i=0; i<task_list.size();i++)
		{
			task_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		task_list.resize(size);
		for(int i=0; i<size;i++)
		{
			task_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_task_list);
		encode(byteArray);
	}
};

struct req_farm_task_list : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_farm_task_list);
		encode(byteArray);
	}
};

struct notify_farm_task_list : public INetPacket
{
	std::vector<player_task> task_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(task_list.size());
		for(size_t i=0; i<task_list.size();i++)
		{
			task_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		task_list.resize(size);
		for(int i=0; i<size;i++)
		{
			task_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_task_list);
		encode(byteArray);
	}
};

struct req_track_task : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_track_task);
		encode(byteArray);
	}
};

struct notify_track_task : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_track_task);
		encode(byteArray);
	}
};

struct req_get_track_list : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_track_list);
		encode(byteArray);
	}
};

struct notify_get_track_list : public INetPacket
{
	std::vector<player_task> task_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(task_list.size());
		for(size_t i=0; i<task_list.size();i++)
		{
			task_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		task_list.resize(size);
		for(int i=0; i<size;i++)
		{
			task_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_track_list);
		encode(byteArray);
	}
};

struct req_cancel_track_task : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_cancel_track_task);
		encode(byteArray);
	}
};

struct notify_cancel_track_task : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_cancel_track_task);
		encode(byteArray);
	}
};

struct req_give_up_task : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_give_up_task);
		encode(byteArray);
	}
};

struct notify_give_up_task : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_give_up_task);
		encode(byteArray);
	}
};

struct notify_task_complete : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_task_complete);
		encode(byteArray);
	}
};

struct notify_give_task : public INetPacket
{
	player_task task;
	void encode(ByteArray &byteArray)
	{
		task.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		task.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_give_task);
		encode(byteArray);
	}
};

struct req_complete_task : public INetPacket
{
	int task_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(task_id);
	}

	void decode(ByteArray &byteArray)
	{
		task_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_complete_task);
		encode(byteArray);
	}
};

struct notify_give_farm_task : public INetPacket
{
	player_task task;
	void encode(ByteArray &byteArray)
	{
		task.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		task.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_give_farm_task);
		encode(byteArray);
	}
};

struct req_farm_complete_task : public INetPacket
{
	int task_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(task_id);
	}

	void decode(ByteArray &byteArray)
	{
		task_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_farm_complete_task);
		encode(byteArray);
	}
};

struct notify_farm_task_complete : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_farm_task_complete);
		encode(byteArray);
	}
};

struct notify_mail_not_read : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_mail_not_read);
		encode(byteArray);
	}
};

struct notify_add_mail : public INetPacket
{
	mail_info mail_data;
	void encode(ByteArray &byteArray)
	{
		mail_data.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		mail_data.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_add_mail);
		encode(byteArray);
	}
};

struct req_delete_mail : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_delete_mail);
		encode(byteArray);
	}
};

struct notify_delete_mail : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_delete_mail);
		encode(byteArray);
	}
};

struct req_get_mail_item : public INetPacket
{
	uint64 id;
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_mail_item);
		encode(byteArray);
	}
};

struct notify_delete_mail_item : public INetPacket
{
	uint64 id;
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_delete_mail_item);
		encode(byteArray);
	}
};

struct req_mail_list : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_mail_list);
		encode(byteArray);
	}
};

struct notify_mail_list : public INetPacket
{
	std::vector<mail_info> mails;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(mails.size());
		for(size_t i=0; i<mails.size();i++)
		{
			mails[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		mails.resize(size);
		for(int i=0; i<size;i++)
		{
			mails[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_mail_list);
		encode(byteArray);
	}
};

struct req_mail_content : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_mail_content);
		encode(byteArray);
	}
};

struct notify_mail_content : public INetPacket
{
	uint64 id;
	std::string content;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_string(content);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		content = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_mail_content);
		encode(byteArray);
	}
};

struct req_know_new_mail_title : public INetPacket
{
	std::vector<uint64> id_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(id_list.size());
		for(size_t i=0; i<id_list.size();i++)
		{
			byteArray.write_uint64(id_list[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		id_list.reserve(size);
		for(int i=0; i<size;i++)
		{
			id_list.push_back(byteArray.read_uint64());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_know_new_mail_title);
		encode(byteArray);
	}
};

struct req_reject_mail : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_reject_mail);
		encode(byteArray);
	}
};

struct req_send_mail : public INetPacket
{
	std::string recver_account;
	std::string recver_name;
	std::string title;
	std::string content;
	std::vector<uint64> items;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(recver_account);
		byteArray.write_string(recver_name);
		byteArray.write_string(title);
		byteArray.write_string(content);
		byteArray.write_uint16(items.size());
		for(size_t i=0; i<items.size();i++)
		{
			byteArray.write_uint64(items[i]);
		}

	}

	void decode(ByteArray &byteArray)
	{
		recver_account = byteArray.read_string();
		recver_name = byteArray.read_string();
		title = byteArray.read_string();
		content = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		items.reserve(size);
		for(int i=0; i<size;i++)
		{
			items.push_back(byteArray.read_uint64());
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_send_mail);
		encode(byteArray);
	}
};

struct notify_send_mail_success : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_send_mail_success);
		encode(byteArray);
	}
};

struct notify_reject_mail_success : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_reject_mail_success);
		encode(byteArray);
	}
};

struct req_open_ui : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_open_ui);
		encode(byteArray);
	}
};

struct notify_open_ui : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_open_ui);
		encode(byteArray);
	}
};

struct req_sys_time : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_sys_time);
		encode(byteArray);
	}
};

struct notify_day_or_night : public INetPacket
{
	int flag;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(flag);
	}

	void decode(ByteArray &byteArray)
	{
		flag = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_day_or_night);
		encode(byteArray);
	}
};

struct notify_sys_time : public INetPacket
{
	stime sys_time;
	void encode(ByteArray &byteArray)
	{
		sys_time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		sys_time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_sys_time);
		encode(byteArray);
	}
};

struct notify_wallow_time : public INetPacket
{
	int wallow_seconds;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(wallow_seconds);
	}

	void decode(ByteArray &byteArray)
	{
		wallow_seconds = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_wallow_time);
		encode(byteArray);
	}
};

struct notify_player_health_status : public INetPacket
{
	std::string account;
	int status;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(status);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		status = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_health_status);
		encode(byteArray);
	}
};

struct notify_disease_special_event : public INetPacket
{
	int special_event_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(special_event_id);
	}

	void decode(ByteArray &byteArray)
	{
		special_event_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_disease_special_event);
		encode(byteArray);
	}
};

struct req_start_buy_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_buy_house);
		encode(byteArray);
	}
};

struct notify_start_buy_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_buy_house);
		encode(byteArray);
	}
};

struct req_end_buy_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_buy_house);
		encode(byteArray);
	}
};

struct notify_end_buy_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_buy_house);
		encode(byteArray);
	}
};

struct req_buy_house : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_house);
		encode(byteArray);
	}
};

struct notify_buy_house : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_buy_house);
		encode(byteArray);
	}
};

struct req_buy_house_replace : public INetPacket
{
	int type;
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_house_replace);
		encode(byteArray);
	}
};

struct req_buy_house_add : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_buy_house_add);
		encode(byteArray);
	}
};

struct req_deposit_money_in_depot : public INetPacket
{
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_deposit_money_in_depot);
		encode(byteArray);
	}
};

struct req_withdraw_money_in_depot : public INetPacket
{
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_withdraw_money_in_depot);
		encode(byteArray);
	}
};

struct notify_player_depot : public INetPacket
{
	int money;
	std::vector<pack_grid> grid_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(money);
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		money = byteArray.read_int();
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_player_depot);
		encode(byteArray);
	}
};

struct req_start_domestic : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_domestic);
		encode(byteArray);
	}
};

struct notify_start_domestic : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_domestic);
		encode(byteArray);
	}
};

struct req_domestic_service : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_domestic_service);
		encode(byteArray);
	}
};

struct notify_domestic_result : public INetPacket
{
	int id;
	int change_house_clean;
	int cost_money;
	int money_rate;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
		byteArray.write_int(change_house_clean);
		byteArray.write_int(cost_money);
		byteArray.write_int(money_rate);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
		change_house_clean = byteArray.read_int();
		cost_money = byteArray.read_int();
		money_rate = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_domestic_result);
		encode(byteArray);
	}
};

struct req_end_domestic : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_domestic);
		encode(byteArray);
	}
};

struct notify_end_domestic : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_domestic);
		encode(byteArray);
	}
};

struct req_walk_for_pick_magic_box : public INetPacket
{
	point curr_pos;
	point dest_pos;
	uint64 box_id;
	int move_type;
	void encode(ByteArray &byteArray)
	{
		curr_pos.encode(byteArray);

		dest_pos.encode(byteArray);

		byteArray.write_uint64(box_id);
		byteArray.write_int(move_type);
	}

	void decode(ByteArray &byteArray)
	{
		curr_pos.decode(byteArray);

		dest_pos.decode(byteArray);

		box_id = byteArray.read_uint64();
		move_type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_walk_for_pick_magic_box);
		encode(byteArray);
	}
};

struct req_pick_magic_box : public INetPacket
{
	uint64 box_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(box_id);
	}

	void decode(ByteArray &byteArray)
	{
		box_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_pick_magic_box);
		encode(byteArray);
	}
};

struct req_client_empty_box_positions : public INetPacket
{
	int need_n;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(need_n);
	}

	void decode(ByteArray &byteArray)
	{
		need_n = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_client_empty_box_positions);
		encode(byteArray);
	}
};

struct notify_server_empty_box_positions : public INetPacket
{
	std::vector<grid_pos> pos_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(pos_list.size());
		for(size_t i=0; i<pos_list.size();i++)
		{
			pos_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		pos_list.resize(size);
		for(int i=0; i<size;i++)
		{
			pos_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_server_empty_box_positions);
		encode(byteArray);
	}
};

struct notify_house_magic_box : public INetPacket
{
	std::vector<magic_box> box_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(box_list.size());
		for(size_t i=0; i<box_list.size();i++)
		{
			box_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		box_list.resize(size);
		for(int i=0; i<size;i++)
		{
			box_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_magic_box);
		encode(byteArray);
	}
};

struct notify_del_house_magic_box : public INetPacket
{
	uint64 box_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(box_id);
	}

	void decode(ByteArray &byteArray)
	{
		box_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_del_house_magic_box);
		encode(byteArray);
	}
};

struct notify_add_house_magic_box : public INetPacket
{
	std::vector<magic_box> box_list;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(box_list.size());
		for(size_t i=0; i<box_list.size();i++)
		{
			box_list[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		box_list.resize(size);
		for(int i=0; i<size;i++)
		{
			box_list[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_add_house_magic_box);
		encode(byteArray);
	}
};

struct notify_pick_magic_box_gain : public INetPacket
{
	int type;
	int item_id;
	int money;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_int(item_id);
		byteArray.write_int(money);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		item_id = byteArray.read_int();
		money = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_pick_magic_box_gain);
		encode(byteArray);
	}
};

struct notify_friend_garbage : public INetPacket
{
	std::string friend_account;
	int n;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(friend_account);
		byteArray.write_int(n);
	}

	void decode(ByteArray &byteArray)
	{
		friend_account = byteArray.read_string();
		n = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_friend_garbage);
		encode(byteArray);
	}
};

struct req_start_gift_service : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_gift_service);
		encode(byteArray);
	}
};

struct notify_start_gift_service : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_gift_service);
		encode(byteArray);
	}
};

struct req_end_gift_service : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_gift_service);
		encode(byteArray);
	}
};

struct notify_end_gift_service : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_gift_service);
		encode(byteArray);
	}
};

struct req_pack_gift_box : public INetPacket
{
	int pack1_type;
	int index1;
	int pack2_type;
	int index2;
	gift_box gift;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(pack1_type);
		byteArray.write_int(index1);
		byteArray.write_int(pack2_type);
		byteArray.write_int(index2);
		gift.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		pack1_type = byteArray.read_int();
		index1 = byteArray.read_int();
		pack2_type = byteArray.read_int();
		index2 = byteArray.read_int();
		gift.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_pack_gift_box);
		encode(byteArray);
	}
};

struct notify_pack_gift_box : public INetPacket
{
	gift_box gift;
	void encode(ByteArray &byteArray)
	{
		gift.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		gift.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_pack_gift_box);
		encode(byteArray);
	}
};

struct req_unpack_gift_box : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_unpack_gift_box);
		encode(byteArray);
	}
};

struct notify_unpack_gift_box : public INetPacket
{
	gift_box gift;
	void encode(ByteArray &byteArray)
	{
		gift.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		gift.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_unpack_gift_box);
		encode(byteArray);
	}
};

struct req_put_gift_box : public INetPacket
{
	uint64 inst_id;
	grid_pos pos;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
		pos.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
		pos.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_put_gift_box);
		encode(byteArray);
	}
};

struct notify_put_gift_box : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_put_gift_box);
		encode(byteArray);
	}
};

struct req_pickup_gift_box : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_pickup_gift_box);
		encode(byteArray);
	}
};

struct notify_pickup_gift_box : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_pickup_gift_box);
		encode(byteArray);
	}
};

struct notify_gift_box : public INetPacket
{
	gift_box gift;
	void encode(ByteArray &byteArray)
	{
		gift.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		gift.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_gift_box);
		encode(byteArray);
	}
};

struct req_get_send_box_vec : public INetPacket
{
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_send_box_vec);
		encode(byteArray);
	}
};

struct notify_get_send_box_vec : public INetPacket
{
	std::vector<gift_box> send_box_vec;
	int total;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(send_box_vec.size());
		for(size_t i=0; i<send_box_vec.size();i++)
		{
			send_box_vec[i].encode(byteArray);
		}

		byteArray.write_int(total);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		send_box_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			send_box_vec[i].decode(byteArray);
		}
		}

		total = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_send_box_vec);
		encode(byteArray);
	}
};

struct req_get_receive_box_vec : public INetPacket
{
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_receive_box_vec);
		encode(byteArray);
	}
};

struct notify_get_receive_box_vec : public INetPacket
{
	std::vector<gift_box> receive_box_vec;
	int total;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(receive_box_vec.size());
		for(size_t i=0; i<receive_box_vec.size();i++)
		{
			receive_box_vec[i].encode(byteArray);
		}

		byteArray.write_int(total);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		receive_box_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			receive_box_vec[i].decode(byteArray);
		}
		}

		total = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_receive_box_vec);
		encode(byteArray);
	}
};

struct req_get_viewed_box_vec : public INetPacket
{
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_viewed_box_vec);
		encode(byteArray);
	}
};

struct notify_get_viewed_box_vec : public INetPacket
{
	std::vector<gift_box> viewed_box_vec;
	int total;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(viewed_box_vec.size());
		for(size_t i=0; i<viewed_box_vec.size();i++)
		{
			viewed_box_vec[i].encode(byteArray);
		}

		byteArray.write_int(total);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		viewed_box_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			viewed_box_vec[i].decode(byteArray);
		}
		}

		total = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_viewed_box_vec);
		encode(byteArray);
	}
};

struct gift_express : public INetPacket
{
	int type;
	uint64 inst_id;
	std::string sender;
	std::string sender_name;
	std::string receiver;
	std::string receiver_name;
	std::vector<pack_grid> grid_vec;
	int card_id;
	std::string comment;
	stime send_date;
	stime date;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
		byteArray.write_uint64(inst_id);
		byteArray.write_string(sender);
		byteArray.write_string(sender_name);
		byteArray.write_string(receiver);
		byteArray.write_string(receiver_name);
		byteArray.write_uint16(grid_vec.size());
		for(size_t i=0; i<grid_vec.size();i++)
		{
			grid_vec[i].encode(byteArray);
		}

		byteArray.write_int(card_id);
		byteArray.write_string(comment);
		send_date.encode(byteArray);

		date.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
		inst_id = byteArray.read_uint64();
		sender = byteArray.read_string();
		sender_name = byteArray.read_string();
		receiver = byteArray.read_string();
		receiver_name = byteArray.read_string();
		{
		UINT16 size = byteArray.read_uint16();
		grid_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			grid_vec[i].decode(byteArray);
		}
		}

		card_id = byteArray.read_int();
		comment = byteArray.read_string();
		send_date.decode(byteArray);

		date.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_gift_express);
		encode(byteArray);
	}
};

struct req_send_gift_express : public INetPacket
{
	int pack1_type;
	int index1;
	int pack2_type;
	int index2;
	gift_express gift;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(pack1_type);
		byteArray.write_int(index1);
		byteArray.write_int(pack2_type);
		byteArray.write_int(index2);
		gift.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		pack1_type = byteArray.read_int();
		index1 = byteArray.read_int();
		pack2_type = byteArray.read_int();
		index2 = byteArray.read_int();
		gift.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_send_gift_express);
		encode(byteArray);
	}
};

struct notify_send_gift_express : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_send_gift_express);
		encode(byteArray);
	}
};

struct req_receive_gift_express : public INetPacket
{
	uint64 inst_id;
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_receive_gift_express);
		encode(byteArray);
	}
};

struct notify_receive_gift_express : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_receive_gift_express);
		encode(byteArray);
	}
};

struct req_get_send_express_vec : public INetPacket
{
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_send_express_vec);
		encode(byteArray);
	}
};

struct notify_get_send_express_vec : public INetPacket
{
	std::vector<gift_express> send_express_vec;
	int total;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(send_express_vec.size());
		for(size_t i=0; i<send_express_vec.size();i++)
		{
			send_express_vec[i].encode(byteArray);
		}

		byteArray.write_int(total);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		send_express_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			send_express_vec[i].decode(byteArray);
		}
		}

		total = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_send_express_vec);
		encode(byteArray);
	}
};

struct req_get_receive_express_vec : public INetPacket
{
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_receive_express_vec);
		encode(byteArray);
	}
};

struct notify_express_vec : public INetPacket
{
	std::vector<gift_express> send_express_vec;
	std::vector<gift_express> receive_express_vec;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(send_express_vec.size());
		for(size_t i=0; i<send_express_vec.size();i++)
		{
			send_express_vec[i].encode(byteArray);
		}

		byteArray.write_uint16(receive_express_vec.size());
		for(size_t i=0; i<receive_express_vec.size();i++)
		{
			receive_express_vec[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		send_express_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			send_express_vec[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		receive_express_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			receive_express_vec[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_express_vec);
		encode(byteArray);
	}
};

struct notify_send_express : public INetPacket
{
	gift_express send_express;
	void encode(ByteArray &byteArray)
	{
		send_express.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		send_express.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_send_express);
		encode(byteArray);
	}
};

struct req_send_express_by_subscribe : public INetPacket
{
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_send_express_by_subscribe);
		encode(byteArray);
	}
};

struct notify_get_receive_express_vec : public INetPacket
{
	std::vector<gift_express> receive_express_vec;
	int total;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(receive_express_vec.size());
		for(size_t i=0; i<receive_express_vec.size();i++)
		{
			receive_express_vec[i].encode(byteArray);
		}

		byteArray.write_int(total);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		receive_express_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			receive_express_vec[i].decode(byteArray);
		}
		}

		total = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_receive_express_vec);
		encode(byteArray);
	}
};

struct req_get_viewed_express_vec : public INetPacket
{
	int page_index;
	int page_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(page_index);
		byteArray.write_int(page_count);
	}

	void decode(ByteArray &byteArray)
	{
		page_index = byteArray.read_int();
		page_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_viewed_express_vec);
		encode(byteArray);
	}
};

struct notify_get_viewed_express_vec : public INetPacket
{
	std::vector<gift_express> viewed_express_vec;
	int total;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(viewed_express_vec.size());
		for(size_t i=0; i<viewed_express_vec.size();i++)
		{
			viewed_express_vec[i].encode(byteArray);
		}

		byteArray.write_int(total);
	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		viewed_express_vec.resize(size);
		for(int i=0; i<size;i++)
		{
			viewed_express_vec[i].decode(byteArray);
		}
		}

		total = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_get_viewed_express_vec);
		encode(byteArray);
	}
};

struct req_clean_gift_log : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_clean_gift_log);
		encode(byteArray);
	}
};

struct notify_show_picture : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_show_picture);
		encode(byteArray);
	}
};

struct req_start_work : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_start_work);
		encode(byteArray);
	}
};

struct notify_start_work : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_start_work);
		encode(byteArray);
	}
};

struct req_work : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_work);
		encode(byteArray);
	}
};

struct notify_work_result : public INetPacket
{
	int id;
	int change_clean;
	int change_health;
	int change_charm;
	int change_active;
	int work_money;
	int event_id;
	int reward_money;
	int reward_item;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
		byteArray.write_int(change_clean);
		byteArray.write_int(change_health);
		byteArray.write_int(change_charm);
		byteArray.write_int(change_active);
		byteArray.write_int(work_money);
		byteArray.write_int(event_id);
		byteArray.write_int(reward_money);
		byteArray.write_int(reward_item);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
		change_clean = byteArray.read_int();
		change_health = byteArray.read_int();
		change_charm = byteArray.read_int();
		change_active = byteArray.read_int();
		work_money = byteArray.read_int();
		event_id = byteArray.read_int();
		reward_money = byteArray.read_int();
		reward_item = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_work_result);
		encode(byteArray);
	}
};

struct req_end_work : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_work);
		encode(byteArray);
	}
};

struct notify_end_work : public INetPacket
{
	int id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_work);
		encode(byteArray);
	}
};

struct req_get_weibo_access_secret : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_get_weibo_access_secret);
		encode(byteArray);
	}
};

struct notify_weibo_access_secret : public INetPacket
{
	std::string tx_access_key;
	std::string tx_access_secret;
	std::string sina_access_key;
	std::string sina_access_secret;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(tx_access_key);
		byteArray.write_string(tx_access_secret);
		byteArray.write_string(sina_access_key);
		byteArray.write_string(sina_access_secret);
	}

	void decode(ByteArray &byteArray)
	{
		tx_access_key = byteArray.read_string();
		tx_access_secret = byteArray.read_string();
		sina_access_key = byteArray.read_string();
		sina_access_secret = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_weibo_access_secret);
		encode(byteArray);
	}
};

struct req_save_access_secret : public INetPacket
{
	std::string tx_access_key;
	std::string tx_access_secret;
	std::string sina_access_key;
	std::string sina_access_secret;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(tx_access_key);
		byteArray.write_string(tx_access_secret);
		byteArray.write_string(sina_access_key);
		byteArray.write_string(sina_access_secret);
	}

	void decode(ByteArray &byteArray)
	{
		tx_access_key = byteArray.read_string();
		tx_access_secret = byteArray.read_string();
		sina_access_key = byteArray.read_string();
		sina_access_secret = byteArray.read_string();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_save_access_secret);
		encode(byteArray);
	}
};

struct req_preview_house : public INetPacket
{
	std::string account;
	uint64 inst_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_uint64(inst_id);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		inst_id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_preview_house);
		encode(byteArray);
	}
};

struct notify_preview_house : public INetPacket
{
	house_data house;
	void encode(ByteArray &byteArray)
	{
		house.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		house.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_preview_house);
		encode(byteArray);
	}
};

struct req_preview_shop_house : public INetPacket
{
	int house_id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(house_id);
	}

	void decode(ByteArray &byteArray)
	{
		house_id = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_preview_shop_house);
		encode(byteArray);
	}
};

struct req_end_preview_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_end_preview_house);
		encode(byteArray);
	}
};

struct notify_end_preview_house : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_end_preview_house);
		encode(byteArray);
	}
};

struct bottle_info : public INetPacket
{
	uint64 id;
	std::string account;
	int type;
	std::string msg;
	stime time;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
		byteArray.write_string(account);
		byteArray.write_int(type);
		byteArray.write_string(msg);
		time.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
		account = byteArray.read_string();
		type = byteArray.read_int();
		msg = byteArray.read_string();
		time.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_bottle_info);
		encode(byteArray);
	}
};

struct req_del_bottle_log : public INetPacket
{
	uint64 id;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint64(id);
	}

	void decode(ByteArray &byteArray)
	{
		id = byteArray.read_uint64();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_del_bottle_log);
		encode(byteArray);
	}
};

struct req_drop_bottle : public INetPacket
{
	std::string msg;
	int type;
	int sex;
	int account_visible;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(msg);
		byteArray.write_int(type);
		byteArray.write_int(sex);
		byteArray.write_int(account_visible);
	}

	void decode(ByteArray &byteArray)
	{
		msg = byteArray.read_string();
		type = byteArray.read_int();
		sex = byteArray.read_int();
		account_visible = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_drop_bottle);
		encode(byteArray);
	}
};

struct notify_drop_bottle : public INetPacket
{
	bottle_info bottle;
	void encode(ByteArray &byteArray)
	{
		bottle.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		bottle.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_drop_bottle);
		encode(byteArray);
	}
};

struct req_pick_bottle : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_pick_bottle);
		encode(byteArray);
	}
};

struct notify_pick_bottle : public INetPacket
{
	bottle_info bottle;
	void encode(ByteArray &byteArray)
	{
		bottle.encode(byteArray);

	}

	void decode(ByteArray &byteArray)
	{
		bottle.decode(byteArray);

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_pick_bottle);
		encode(byteArray);
	}
};

struct req_all_bottle_info : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_all_bottle_info);
		encode(byteArray);
	}
};

struct notify_bottles_info : public INetPacket
{
	std::vector<bottle_info> pick_bottles;
	std::vector<bottle_info> drop_bottles;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(pick_bottles.size());
		for(size_t i=0; i<pick_bottles.size();i++)
		{
			pick_bottles[i].encode(byteArray);
		}

		byteArray.write_uint16(drop_bottles.size());
		for(size_t i=0; i<drop_bottles.size();i++)
		{
			drop_bottles[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		pick_bottles.resize(size);
		for(int i=0; i<size;i++)
		{
			pick_bottles[i].decode(byteArray);
		}
		}

		{
		UINT16 size = byteArray.read_uint16();
		drop_bottles.resize(size);
		for(int i=0; i<size;i++)
		{
			drop_bottles[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_bottles_info);
		encode(byteArray);
	}
};

struct notify_today_bottle_info : public INetPacket
{
	int year;
	int month;
	int day;
	int pick_count;
	int drop_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(year);
		byteArray.write_int(month);
		byteArray.write_int(day);
		byteArray.write_int(pick_count);
		byteArray.write_int(drop_count);
	}

	void decode(ByteArray &byteArray)
	{
		year = byteArray.read_int();
		month = byteArray.read_int();
		day = byteArray.read_int();
		pick_count = byteArray.read_int();
		drop_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_today_bottle_info);
		encode(byteArray);
	}
};

struct notify_open_float_bottle_pool : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_open_float_bottle_pool);
		encode(byteArray);
	}
};

struct notify_house_decoration : public INetPacket
{
	std::string account;
	int decoration;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(decoration);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		decoration = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_decoration);
		encode(byteArray);
	}
};

struct notify_new_day_reset : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_new_day_reset);
		encode(byteArray);
	}
};

struct notify_total_flower_count : public INetPacket
{
	std::string account;
	int flower_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(account);
		byteArray.write_int(flower_count);
	}

	void decode(ByteArray &byteArray)
	{
		account = byteArray.read_string();
		flower_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_total_flower_count);
		encode(byteArray);
	}
};

struct req_send_flower : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_send_flower);
		encode(byteArray);
	}
};

struct notify_send_flower_success : public INetPacket
{
	player_info player;
	std::vector<pack_grid> body;
	void encode(ByteArray &byteArray)
	{
		player.encode(byteArray);

		byteArray.write_uint16(body.size());
		for(size_t i=0; i<body.size();i++)
		{
			body[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		player.decode(byteArray);

		{
		UINT16 size = byteArray.read_uint16();
		body.resize(size);
		for(int i=0; i<size;i++)
		{
			body[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_send_flower_success);
		encode(byteArray);
	}
};

struct req_flower_logs : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_flower_logs);
		encode(byteArray);
	}
};

struct notify_flower_logs : public INetPacket
{
	std::vector<flower_log> logs;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(logs.size());
		for(size_t i=0; i<logs.size();i++)
		{
			logs[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		logs.resize(size);
		for(int i=0; i<size;i++)
		{
			logs[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_flower_logs);
		encode(byteArray);
	}
};

struct notify_house_flower_change : public INetPacket
{
	std::string house_owner;
	int flower_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(house_owner);
		byteArray.write_int(flower_count);
	}

	void decode(ByteArray &byteArray)
	{
		house_owner = byteArray.read_string();
		flower_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_flower_change);
		encode(byteArray);
	}
};

struct req_send_egg : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_send_egg);
		encode(byteArray);
	}
};

struct notify_send_egg_success : public INetPacket
{
	player_info player;
	std::vector<pack_grid> body;
	void encode(ByteArray &byteArray)
	{
		player.encode(byteArray);

		byteArray.write_uint16(body.size());
		for(size_t i=0; i<body.size();i++)
		{
			body[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		player.decode(byteArray);

		{
		UINT16 size = byteArray.read_uint16();
		body.resize(size);
		for(int i=0; i<size;i++)
		{
			body[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_send_egg_success);
		encode(byteArray);
	}
};

struct req_egg_logs : public INetPacket
{
	void encode(ByteArray &byteArray)
	{
	}

	void decode(ByteArray &byteArray)
	{
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_egg_logs);
		encode(byteArray);
	}
};

struct notify_egg_logs : public INetPacket
{
	std::vector<egg_log> logs;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_uint16(logs.size());
		for(size_t i=0; i<logs.size();i++)
		{
			logs[i].encode(byteArray);
		}

	}

	void decode(ByteArray &byteArray)
	{
		{
		UINT16 size = byteArray.read_uint16();
		logs.resize(size);
		for(int i=0; i<size;i++)
		{
			logs[i].decode(byteArray);
		}
		}

	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_egg_logs);
		encode(byteArray);
	}
};

struct notify_house_egg_change : public INetPacket
{
	std::string house_owner;
	int egg_count;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_string(house_owner);
		byteArray.write_int(egg_count);
	}

	void decode(ByteArray &byteArray)
	{
		house_owner = byteArray.read_string();
		egg_count = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_house_egg_change);
		encode(byteArray);
	}
};

struct req_is_active_game : public INetPacket
{
	int type;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(type);
	}

	void decode(ByteArray &byteArray)
	{
		type = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_req_is_active_game);
		encode(byteArray);
	}
};

struct notify_active_game_result : public INetPacket
{
	int result;
	void encode(ByteArray &byteArray)
	{
		byteArray.write_int(result);
	}

	void decode(ByteArray &byteArray)
	{
		result = byteArray.read_int();
	}

	void build(ByteArray &byteArray)
	{
		byteArray.write_uint16(NetMsgType::msg_notify_active_game_result);
		encode(byteArray);
	}
};
