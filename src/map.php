<!DOCTYPE html>
<html>

<head>
    <style>
        #map {
            height: 800px;
            width: 100%;
        }
    </style>
</head>

<body>

    <?php
  $servername = "localhost"; 
  $username = "xxxxxxxxxxx"; // username for your database
  $password = "xxxxxxxxxxx"; // password for your database
  $dbname = "xxxxxxxxxxx"; // database name

$conn = mysqli_connect($servername,$username,$password)  or die(mysqli_error());
mysqli_select_db($conn, $dbname);

$sql_read = "SELECT * FROM Date";

$result = mysqli_query($conn, $sql_read);
if(! $result )
{
  die('Could not read data: ' . mysqli_error());
}
    ?>


    <h3 style="text-align: center">Map</h3>
    <div id="map"></div>
    <script>
        function initMap() {

            var centru = {
                lat: 45,
                lng: 25
            };

            var map = new google.maps.Map(document.getElementById('map'), {
                zoom: 4,
                center: centru
            });

            <?php
            
            while($row = mysqli_fetch_array($result)) {
	       $lat = $row['latitudine'];
	       $lng = $row['longitudine'];   
           echo "var marker = new google.maps.Marker({position:{lat:$lat,lng:$lng},map: map});\n" ;  
            }
               
           ?>
        }
    </script>
    <script async defer src="https://maps.googleapis.com/maps/api/js?key=YOURKEY&callback=initMap">
    </script>
</body>

</html>
