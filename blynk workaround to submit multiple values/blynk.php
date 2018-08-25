<?php
/*
	This scripts submits multiple tehybug values to multiple pins at blynk, you can place it at any webhost which supports php, even a free host should work, just make sure that they have curl lib enabled.
	
	define the auth token below or add the token as auth_token=yourtokenvalue to the url,
	configure http GET request like this:
	http://tehybug.com/blynk/auth_token/tokenvaluegoeshere/?pins=V0,V1&values=%temp%,%humi%
	or
	http://domain.com/blynk.php?auth_token=yourtokenvalue&pins=V0,V1&values=%temp%,%humi%
	
	configure http POST request for your tehybug with post body like:
	[{'pin':'V0','value':'%temp%'},{'pin':'V1','value':'%humi%'},{'pin':'V2','value':'%qfe%'}]
	for multiple pins and values
	point the url to this script:
	http://tehybug.com/blynk/auth_token/tokenvaluegoeshere/
	or
	http://domain.com/blynk.php?auth_token=yourtokenvalue
	
	
	In case you want to receive push notification when some environmental parameters are below or above defined values you will have to add following parameters.
	!Requires the notification widget
	
	For a GET request add to url:
	
	&push_over_pins=V0,V1&push_over_values=10.1,20.2 
	this will send a push notification when pin V0 value is over 10.1 or pin V1 value is over 20.2
	full url example: 
	http://tehybug.com/blynk/auth_token/tokenvaluegoeshere/?pins=V0,V1&values=%temp%,%humi%&push_over_pins=V0,V1&push_over_values=10.1,20.2
	or 
	http://domain.com/blynk.php?auth_token=yourtokenvalue&pins=V0,V1&values=%temp%,%humi%&push_over_pins=V0,V1&push_over_values=10.1,20.2
	
	&push_below_pins=V2&push_below_values=20.2 
	this will send a push notification when pin V2 value is below 20.2
	full url example: 
	http://tehybug.com/blynk/auth_token/tokenvaluegoeshere/?pins=V2&values=%temp%&push_below_pins=V2&&push_below_values=20.2 
	or 
	http://domain.com/blynk.php?auth_token=yourtokenvalue&pins=V2&values=%temp%&push_over_pins=V2&push_over_values=20.2
	
	You can use push over and push below parameters at the same time.
	
	For a POST request the post body should look like:
	[{'pin':'V0','value':'%temp%', 'push_over':50,'push_below':5},{'pin':'V1','value':'%humi%'},{'pin':'V2','value':'%qfe%'}]
	
	this will send a push notification when the temperature value is over 50 or below 5 degrees
	
	
*/
//for tehybug post
if(!empty($_REQUEST['auth_token']))
{
	$auth_token = $_GET['auth_token'];
}
else
{
	$auth_token = '4ae3851817194e2596cf1b7103603ef8'; // define your auth token here in case you dont add it to the url as auth_token=yourtokenvalue
}
if(!empty($_REQUEST['pins']))
{
	$pins_arr = explode(',', $_REQUEST['pins']);
	$values_arr = explode(',', $_REQUEST['values']);
	
	$push_pins = array();
	$push_over_pins_arr = explode(',', $_REQUEST['push_over_pins']);
	$push_below_pins_arr = explode(',', $_REQUEST['push_below_pins']);
	$push_over_values_arr = explode(',', $_REQUEST['push_over_values']);
	$push_below_values_arr = explode(',', $_REQUEST['push_below_values']);
	for($i=0;$i<count($pins_arr);$i++)
	{
		$blynk_url = 'http://blynk-cloud.com/'.$auth_token.'/update/'.$pins_arr[$i].'?value='.$values_arr[$i];
		curl_get($blynk_url);
		echo '<br>';
		
		echo 'Pin <b>'.$pins_arr[$i].'</b> is set to <b>'.$values_arr[$i].'</b><br>';
		$push_pins[$pins_arr[$i]] = $values_arr[$i];
	}
	for($i=0;$i<count($push_over_pins_arr);$i++)
	{
		$blynk_url = 'http://blynk-cloud.com/'.$auth_token.'/notify';
		echo $push_pins[$push_over_pins_arr[$i]];
		if(!empty($push_pins[$push_over_pins_arr[$i]]) && $push_pins[$push_over_pins_arr[$i]] > $push_over_values_arr[$i])
		{
			curl_post($blynk_url,'Pin '.$push_over_pins_arr[$i].' value is over '.$push_over_values_arr[$i]); // send push notification
		}
	}
	for($i=0;$i<count($push_below_pins_arr);$i++)
	{
		$blynk_url = 'http://blynk-cloud.com/'.$auth_token.'/notify';
		if(!empty($push_pins[$push_below_pins_arr[$i]]) && $push_pins[$push_below_pins_arr[$i]] < $push_below_values_arr[$i])
		{
			curl_post($blynk_url,'Pin '.$push_below_pins_arr[$i].' value is below '.$push_below_values_arr[$i]); // send push notification
		}
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
		curl_get($blynk_url);
		echo '<br>';
		
		echo 'Pin <b>'.$data['pin'].'</b> is set to <b>'.$data['value'].'</b><br>';
		
		if(!empty($data['push_over']) && $data['push_over'] < $data['value'])
		{
			curl_post($blynk_url,'Pin '.$data['pin'].' value is over '.$data['push_over']); // send push notification
		}
		if(!empty($data['push_below']) && $data['push_below'] > $data['value'])
		{
			curl_post($blynk_url,'Pin '.$data['pin'].' value is below '.$data['push_below']); // send push notification
		}
		
	}
}

function curl_get($blynk_url)
{
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $blynk_url);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
	curl_setopt($ch, CURLOPT_HEADER, FALSE);
	$response = curl_exec($ch);
	curl_close($ch);
	var_dump($response);
	//optionally you can use file_get_contents instead of curl like:
	//file_get_contents($blynk_url);
}

function curl_post($blynk_url,$body)
{
	$ch = curl_init();

	curl_setopt($ch, CURLOPT_URL, $blynk_url);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
	curl_setopt($ch, CURLOPT_HEADER, FALSE);
	
	curl_setopt($ch, CURLOPT_POST, TRUE);
	
	curl_setopt($ch, CURLOPT_POSTFIELDS, '{
	  "body": "'.$body.'"
	}');
	
	curl_setopt($ch, CURLOPT_HTTPHEADER, array(
	  "Content-Type: application/json"
	));
	
	$response = curl_exec($ch);
	curl_close($ch);
	
	var_dump($response);
	echo $body.'<br>';
}



?>