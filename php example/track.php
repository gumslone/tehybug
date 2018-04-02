<?php

/*
	point your tehybug to this file
*/
echo 'OK';
if(!empty($_REQUEST['sensor']))
{

	if(!file_exists('./data/'.$_REQUEST['sensor'].''))
	{
		mkdir('./data/'.$_REQUEST['sensor'].'');
	}
	$sensor_dir = $_REQUEST['sensor'].'/';
	
	
	file_put_contents('./data/'.$sensor_dir.date("Y-m-d").'_sensor_data.txt', 'time='.time().'&'.$_SERVER['QUERY_STRING']."\n", FILE_APPEND | LOCK_EX);
}
?>