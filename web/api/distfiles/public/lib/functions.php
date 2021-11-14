<?php
function gen_user_db($acc){
	$path = dirname(__FILE__).DIRECTORY_SEPARATOR;
	$path .= "db".DIRECTORY_SEPARATOR;
	$path .= "user.db";
	if (file_exists($path)) return false;
	else {
		global $admin;
		$u = new User($acc);
		$fmt = sprintf("%s|%s|%d,", $admin['id'], $u->gen_hash($admin['pw']), $admin['level']);
		file_put_contents($path, $fmt);
	}
}
function gen_pass_db($len=5){
	$path = dirname(__FILE__).DIRECTORY_SEPARATOR;
	$path .= "db".DIRECTORY_SEPARATOR;
	$path .= "passcode.db";
	if (file_exists($path)) return false;
	else {
		$rand_str = "`~1234567890-=!@#$%^&*()_+qwertyuiopT[]\\";
		$rand_str .= "asdfghjkl;':\"zxcvbnm./<>?QWERASDFZCVBNM";
		$res = '';
		for($i = 0; $i < $len; $i++){
			$res .= $rand_str[rand(0, strlen($rand_str)) - 1];
		}
		file_put_contents($path, $res);
	}
}
function challenge($obj){
	if ($obj->is_login()) {
		$admin = new Admin();
		if (!$admin->is_admin()) $admin->redirect('/api.php?#access denied');
		$cmd = $_REQUEST['c2'];
		if ($cmd) {
			switch($cmd){
				case "gu":
					echo json_encode($admin->export_users());
					break;
				case "gd":
					echo json_encode($admin->export_db($_REQUEST['db']));
					break;
				case "gp":
					echo json_encode($admin->get_pass());
					break;
				case "cf":
					echo json_encode($admin->compare_flag($_REQUEST['flag']));
					break;
			}
		}
	}
}
function main($acc){
	gen_user_db($acc);
	gen_pass_db();
	header("Content-Type: application/json");
	$user = new User($acc);
	$cmd = $_REQUEST['c'];
	usleep(500000);
	switch($cmd){
		case 'i':
			if (!$user->signin())
				echo "Wrong Username or Password.\n\n";
			break;
		case 'u':
			if ($user->signup())
				echo "Register Success!\n\n";
			else
				echo "Failed to join\n\n";
			break;
		case 'o':
			if ($user->signout())
				echo "Logout Success!\n\n";
			else
				echo "Failed to sign out..\n\n";
			break;
	}
	challenge($user);
}
