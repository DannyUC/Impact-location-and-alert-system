<?php 
$data=$_GET['data'];
$ora=$_GET['ora'];
$lat=$_GET['latitudine']; 
$lng=$_GET['longitudine']; 
 
  $servername = "localhost"; 
  $username = "xxxxxxxxxxx"; // username for your database
  $password = "xxxxxxxxxxx"; // password for your database
  $dbname = "xxxxxxxxxxx"; // database name
	$conn = mysqli_connect($servername,$username,$password,$dbname); 
 
if (!$conn) { 
    		die("Connection failed with database: " . mysqli_connect_error()); 
	} 
	 
if ($conn->connect_error) { 
    die("Connection failed: " . $conn->connect_error); 
  } 
 
   
$sql = "INSERT INTO Date VALUES ('$data','$ora','$lat','$lng')"; 
 
if ($conn->query($sql) === TRUE) { 
    echo "New record created successfully"; 
} else { 
    echo "Error: " . $sql . "<br>" . $conn->error; 
} 
 
$conn->close(); 
?> 
