-include("packet_def.hrl").
%% �ƶ�������
-record(move_data, {
	  pos=#point{x=0.0,y=0.0,z=0.0},    %%�������
	  path,                             %%����·��
	  speed,                            %%�ƶ��ٶ�
	  pos_index                         %%��ǰ���ߵ��ĵ������
	 }).


%%�������� 
-record(scene_data,{
	  scene_name,            %% ��������
	  players = dict:new(),  %% ����б�, һ��dict
	  enter_pos=#point{x=0,y=0,z=0}, %% �����
	  module,                %% ģ����
	  custom_data            %% ���ݻ򹫹��������Զ�����
	 }).
