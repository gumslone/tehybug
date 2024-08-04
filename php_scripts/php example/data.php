<?php
		//show all available sensors
		$scan_folder = './data/'; //sensor data folder
		$sensor_folders = glob($scan_folder.'/*',GLOB_ONLYDIR|GLOB_BRACE);
		if(count($sensor_folders)>0)
		{
			$sensors = array();
			foreach ($sensor_folders as $folder) 
			{
				array_push($sensors, basename($folder));
			}
		}
		print_r($sensors);
		
		//show all available data dates 
		if(count($sensors)>0)
		{
						
			foreach($sensors AS $sensor)
			{
				$files = array_diff(scandir($scan_folder.$sensor), array('.', '..'));
				$files = array_reverse($files);
				$files = array_values( $files );
				$sensor_dates[$sensor] = $files;
			}
			
		}
		
		print_r($sensor_dates);
		
		//show last sensor data
		if(count($sensor_dates)>0)
		{
			foreach($sensors AS $sensor)
			{
				$file = './data/'.$sensor.'/'.$sensor_dates[$sensor][0];
				$contents = file_get_contents($file);
				$lines = explode(PHP_EOL, $contents);
				
				foreach($lines as $line)
				{
					if(!empty($line))
					{
						$values = explode('&', $line);
						for($i=0;$i<3;$i++)
						{
							
							$data = explode('=', $values[$i]);
						
							if($i==0) 
								$out_data[$i][] = $data[1] * 1000;
							else
								$out_data[$i][] = $data[1];
						}
					}
				}
				
				print_r($out_data);
			}
		}

?>