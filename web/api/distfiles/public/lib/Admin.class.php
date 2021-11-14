<?php
class Admin extends User {
	private $sess;
	private $db;
	public function __construct(){
		$this->sess = $_SESSION;
		$this->db['path'] = dirname(__FILE__).DIRECTORY_SEPARATOR;
		$this->db['path'] .= "db".DIRECTORY_SEPARATOR;
		$this->db['path'] .= "passcode.db";
	}
	public function is_admin(){
		if ($this->sess[2]) return true;
	}
	public function export_users(){
		if ($this->is_pass_correct()) {
			$path = dirname(__FILE__).DIRECTORY_SEPARATOR;
			$path .= "db".DIRECTORY_SEPARATOR;
			$path .= "user.db";
			$data = file_get_contents($path);
			$data = explode(',', $data, -1);
			$arr = [];
			for($i = 0; $i < count($data); $i++){
				$tmp = explode('|', $data[$i]);
				$arr[] = $tmp[0];
			}
			return $arr;
		}else 
			return "The passcode does not equal with your input.";
	}
	public function export_db($file){
		if ($this->is_pass_correct()) {
			$path = dirname(__FILE__).DIRECTORY_SEPARATOR;
			$path .= "db".DIRECTORY_SEPARATOR;
			$path .= $file;
			$data = file_get_contents($path);
			$data = explode(',', $data);
			$arr = [];
			for($i = 0; $i < count($data); $i++){
				$arr[] = explode('|', $data[$i]);
			}
			return $arr;
		}else 
			return "The passcode does not equal with your input.";
	}
	public function is_pass_correct(){
		$passcode = $this->get_pass();
		$input = $_REQUEST['pas'];
		if ($input == $passcode) return true;
	}
	public function get_pass(){
		return file_get_contents($this->db['path']);
	}
	public function compare_flag($flag){
		$this->flag = trim(file_get_contents("/flag"));
		if ($this->flag == $flag) return "Yess! That's it!";
		else return "That's not the flag..";
	}
	public function __destruct(){
		$this->signout();
	}
}
