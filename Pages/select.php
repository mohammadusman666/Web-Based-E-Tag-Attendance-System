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

$name = $_GET["name"];

$sql = "SELECT * FROM people WHERE name='$name' LIMIT 1";
$result = $conn->query($sql);

if ($result->num_rows)
{
    $row = $result->fetch_assoc();
    $name = $row['name'];
    $desig = $row['designation'];
    $uid = $row['uid'];

    $sql = "SELECT dat, tim FROM attendance WHERE uid='$uid'";
    $result = $conn->query($sql);

    $heading = "<h2 style='text-align:center;'>" . $name . " (" . $uid . ") (" . $desig . ")</h2>";

    if ($result->num_rows)
    {
        echo "<html>";
        echo "<head><style>";
        echo "body{font-family:arial,sans-serif;}";
        echo "table{border-collapse:collapse;width:100%;}";
        echo "td,th{border:1px solid #dddddd;text-align:center;padding:8px;}";
        echo "tr:nth-child(even){background-color:#dddddd;}";
        echo "</style></head>";
        echo "<body>";
        echo $heading;
        echo "<table>";
        echo "<tr><th>Date</th><th>Time</th></tr>";
        while ($row = $result->fetch_assoc())
        {
            echo "<tr><td>" . $row['dat'] . "</td><td>" . $row['tim'] . "</td></tr>";
        }
        echo "</table></body>";
        echo "</html>";
    }
    else
    {
        echo $heading;
        echo "<p style='text-align:center;'>No Attendance record for this person!</p>";
    }
}
else
{
    echo "<h2 style='text-align:center;font-family:arial,sans-serif;'>";
    echo "No such person exists in the database!";
    echo "</h2>";
}

$conn->close();
?>