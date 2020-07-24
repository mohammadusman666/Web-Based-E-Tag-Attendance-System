<?php
$servername = "localhost";
$username = 'root';
$password = '123456';
$dbname = 'etag';

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$uid = $_POST["uid"];

$sql = "SELECT * FROM people WHERE uid='$uid'";
$result = $conn->query($sql);

if ($result->num_rows)
{
    $date = date("Y-m-d");
    $time = date("h:i:s");

    $sql = "INSERT INTO attendance (uid, dat, tim)
            VALUES ('$uid', '$date', '$time')";

    if ($conn->query($sql) === TRUE)
        echo "OK";
    else
    {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
}
else
{
    echo "Unauthorized";
}

$conn->close();
?>