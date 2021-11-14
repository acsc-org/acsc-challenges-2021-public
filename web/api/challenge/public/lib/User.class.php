<?php
class User {
	private $acc;
	private $db;
	public function __construct($acc){
		$this->acc = $acc;
		$this->db['path'] = dirname(__FILE__).DIRECTORY_SEPARATOR;
		$this->db['path'] .= "db".DIRECTORY_SEPARATOR;
		$this->db['path'] .= "user.db";
		$this->db['fmt'] = sprintf("%s|%s|%d,", $this->acc[0], $this->gen_hash($this->acc[1]), 0);
	}
	public function is_login(){
		if ($_SESSION) return true;
	}
	public function gen_hash($val){
		return hash("ripemd160", $val);
	}
	public function load_db(){
		$data = file_get_contents($this->db['path']);
		$data = explode(',', $data, -1);
		$arr = [];
		for($i = 0; $i < count($data); $i++){
			$arr[] = explode('|', $data[$i]);
		}
		return $arr;
	}
	public function redirect($url, $msg=''){
		$con = "<script type='text/javascript'>".PHP_EOL;
		if ($msg) $con .= "\talert('%s');".PHP_EOL;
		$con .= "\tlocation.href = '%s';".PHP_EOL;
		$con .= "</script>".PHP_EOL;
		header("location: ".$url);
		if ($msg) printf($con, $msg, $url);
		else printf($con, $url);
	}
	public function signin(){
		$data = $this->load_db();
		$bool = false;
		for($i = 0; $i < count($data); $i++){
			if ($data[$i][0] == $this->acc[0] && 
			    $data[$i][1] == $this->gen_hash($this->acc[1])) {
				$bool = true;
				break;
			}
		}
		if ($bool) {
			$_SESSION = [$this->acc[0], $this->acc[1], $this->acc[2]];
			return true;
		}
		else return false;
	}
	public function signup(){
		if (!preg_match("/^[A-Z][0-9a-z]{3,15}$/", $this->acc[0])) return false;
		if (!preg_match("/^[A-Z][0-9A-Za-z]{8,15}$/", $this->acc[1])) return false;
		$data = $this->load_db();
		for($i = 0; $i < count($data); $i++){
			if ($data[$i][0] == $this->acc[0]) return false;
		}
		file_put_contents($this->db['path'], $this->db['fmt'], FILE_APPEND);
		return true;
	}
	public function signout(){
		$_SESSION = [];
		return true;
	}
}
