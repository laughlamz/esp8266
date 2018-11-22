<?php 
include('inc/myconnect.php');

$s = $_GET['s'];

	echo "001";

	if($s != null) {
		$ar = str_split($s);
		mysqli_query($dbc, "UPDATE 1_control SET s1='$ar[0]',s2='$ar[1]',s3='$ar[2]' " );
	}


mysqli_close($dbc);		  
?>
