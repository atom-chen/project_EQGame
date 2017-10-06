-record(player_data, {
	  account = "",
	  basic_data = #player_basic_data{},         % ��һ�����Ϣ
	  player_property=#player_property{},        % �������
	  scene_name,                                % ��������
	  mm_pid,                                    % �м���:���ؽ��̵�PID
	  net_process = [],                          % ������Ϣ������
	  curr_pos=#point{x=0.0,y=0.0,z=0.0},        % ��ҵ�ǰ������
	  dest_pos=#point{x=0.0,y=0.0,z=0.0},        % ���Ŀ�������
	  walk_plan=undefined,                       % ��Ҵ˴����ߵ�Ŀ��(ʹ�üҾߣ����˻����ȵ�)
	  actions = [],                              % ��ǰ����(�༭���ݣ����׵ȵ�)       
	  invite_info,                               % ������Ϣ
	  trade_info,                                % ���������Ϣ
	  work_info,                                 % ����Ϣ
	  domestic_info,                             % ������Ϣ
	  body_action_info,                          % ����������Ϣ
	  body_state=1,                              % �����ɵ�״̬ 1��ʾ����
	  mail_count = 0,                            % ��ǰ�ͻ����ʼ���
	  login_time,                                % ��ҵ�½ʱ��
	  is_first_time_login,                       % �Ƿ��״ε�½
	  player_coin,                               % ����
	  used_furni_inst_id = 0,                    % �Ҿߵ�ʵ��id
	  used_furni_func_id = 0,                    % �Ҿ�ʹ�õĹ���id
	  used_furni_index = 0,                      % ʹ�õļҾ�λ��
	  used_furni_pos = #point{x=0.0,y=0.0,z=0.0},% ʹ�üҾߵ�����	  
	  start_edit_house_time,                     % ʲôʱ��ʼ�༭����
	  wallow_info,                               % �����������Ϣ

	  flower_count=0,                            % �ʻ���                
	  task,                                      % ����

	  bag_grids,                                 % ��Ұ���
	  body_grids,                                % ���װ��
	  depot,                                     % ��Ҳֿ�	  
	  
	  gift,                                      % �������
	  gift_service_type,                         % �����������
	  property_timer_ref                         % ����������ԵĶ�ʱ��
	 }).

