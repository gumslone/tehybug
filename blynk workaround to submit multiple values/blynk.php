<?php
/*
	This scripts submits multiple tehybug values to multiple pins at blynk, you can place it at any webhost which supports php, even a free host should work, just make sure that they have curl lib enabled.
	
	define the auth token below,
	configure http get reuest like this:
	http://tehybug.com/blynk/auth_token/tokenvaluegoeshere/?pins=V0,V1&values=%temp%,%humi%
	or
	http://domain.com/blynk.php?auth_token=yourtokenvalue&pins=V0,V1&values=%temp%,%humi%
	configure http post request for your tehybug with post body like:
	[{'pin':'V0','value':'%temp%'},{'pin':'V1','value':'%humi%'},{'pin':'V2','value':'%qfe%'}]
	for multiple pins and values
	point the url to this script:
	http://domain.com/blynk.php?auth_token=yourtokenvalue
	
*/
//for tehybug post
if(!empty($_REQUEST['auth_token']))
{
	$auth_token = $_GET['auth_token'];
}
else
{
	$auth_token = '4ae3851817194e2596cf1b7103603ef8'; // define your auth token here
}
if(!empty($_REQUEST['pins']))
{
	$pins_arr = explode(',', $_REQUEST['pins']);
	$values_arr = explode(',', $_REQUEST['values']);
	for($i=0;$i<count($pins_arr);$i++)
	{
		$blynk_url = 'http://blynk-cloud.com/'.$auth_token.'/update/'.$pins_arr[$i].'?value='.$values_arr[$i];
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $blynk_url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
		curl_setopt($ch, CURLOPT_HEADER, FALSE);
		$response = curl_exec($ch);
		curl_close($ch);
		var_dump($response);
		echo '<br>';
		//optionally you can use file_get_contents instead of curl like:
		//file_get_contents($blynk_url);
		echo 'Pin <b>'.$pins_arr[$i].'</b> is set to <b>'.$values_arr[$i].'</b><br>';
	}
}
else
{
	$raw_data = file_get_contents("php://input");
	$raw_data = str_replace("'", '"', $raw_data);
	$json_data = json_decode($raw_data,TRUE);
	foreach ($json_data as $data)
	{
		$blynk_url = 'http://blynk-cloud.com/'.$auth_token.'/update/'.$data['pin'].'?value='.$data['value'];
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $blynk_url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
		curl_setopt($ch, CURLOPT_HEADER, FALSE);
		$response = curl_exec($ch);
		curl_close($ch);
		var_dump($response);
		echo '<br>';
		//optionally you can use file_get_contents instead of curl like:
		//file_get_contents($blynk_url);
		echo 'Pin <b>'.$data['pin'].'</b> is set to <b>'.$data['value'].'</b><br>';
	}
}
?>