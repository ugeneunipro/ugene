<?php
/* $server = the IP address or network name of the server
 * $userName = the user to log into the database with
 * $password = the database account password
 * $databaseName = the name of the database to pull data from
 * table structure - colum1 is cas: has text/description - column2 is data has the value
 */
 date_default_timezone_set('Europe/Berlin');
$con = mysqli_connect('localhost', 'user_ustats', '[htyjntym#2') or die('Error connecting to server');

mysqli_select_db($con, 'db_ugene_stats');

// write your SQL query here (you may use parameters from $_GET or $_POST if you need them)
if (! isset($_GET['start']) || ! isset($_GET['end'])) {
    $start_date=date('Y-m-d', strtotime("-1 weeks -1 days"));
    $end_date=date('Y-m-d', strtotime("-1 days"));
}else{
    $start_date=$_GET['start'];
    $end_date=$_GET['end'];
}
$start_date2=date_create($start_date)->modify('-1 weeks')->format('Y-m-d');
$end_date2=date_create($end_date)->modify('-1 weeks')->format('Y-m-d');
$start_date3=date_create($start_date)->modify('-2 weeks')->format('Y-m-d');
$end_date3=date_create($end_date)->modify('-2 weeks')->format('Y-m-d');
$start_date4=date_create($start_date)->modify('-3 weeks')->format('Y-m-d');
$end_date4=date_create($end_date)->modify('-3 weeks')->format('Y-m-d');


$query = mysqli_query($con, "select value_name as counter,SUM(value) as sum from reports INNER JOIN reports_values ON reports.id=reports_values.report_id where value_name='ugeneui launch' AND reports.gmt > DATE('$start_date')  AND reports.gmt <=DATE('$end_date')  AND reports.ip != '212.17.10.114' AND reports.ip != '95.170.150.101';");
$query2  = mysqli_query($con, "select value_name as counter,SUM(value) as sum from reports INNER JOIN reports_values ON reports.id=reports_values.report_id where value_name='ugeneui launch' AND reports.gmt > DATE('$start_date2') AND reports.gmt <=DATE('$end_date2') AND reports.ip != '212.17.10.114' AND reports.ip != '95.170.150.101';");

$query3 = mysqli_query($con, "select value_name as counter,SUM(value) as sum from reports INNER JOIN reports_values ON reports.id=reports_values.report_id where value_name='ugeneui launch' AND reports.gmt > DATE('$start_date3') AND reports.gmt <=DATE('$end_date3') AND reports.ip != '212.17.10.114' AND reports.ip != '95.170.150.101';");
$query4 = mysqli_query($con, "select value_name as counter,SUM(value) as sum from reports INNER JOIN reports_values ON reports.id=reports_values.report_id where value_name='ugeneui launch' AND reports.gmt > DATE('$start_date4') AND reports.gmt <=DATE('$end_date4') AND reports.ip != '212.17.10.114' AND reports.ip != '95.170.150.101';");

$query_users = mysqli_query($con, "select count(DISTINCT user_id) as sum from reports INNER JOIN users ON reports.user_id=users.id where reports.gmt > DATE('$start_date') AND reports.gmt <=DATE('$end_date') AND reports.ip NOT LIKE '%212.17.10.114%' AND users.ip  NOT LIKE '%212.17.10.114%' AND reports.ip != '95.170.150.101' AND reports.ip NOT LIKE '%192.168%' AND users.ip NOT LIKE '%192.168%';");
$query_users2  = mysqli_query($con, "select count(DISTINCT user_id) as sum from reports INNER JOIN users ON reports.user_id=users.id where reports.gmt > DATE('$start_date2') AND reports.gmt <=DATE('$end_date2') AND reports.ip NOT LIKE '%212.17.10.114%' AND users.ip  NOT LIKE '%212.17.10.114%' AND reports.ip != '95.170.150.101' AND reports.ip NOT LIKE '%192.168%' AND users.ip NOT LIKE '%192.168%';");

$query_users3 = mysqli_query($con, "select count(DISTINCT user_id) as sum from reports INNER JOIN users ON reports.user_id=users.id where reports.gmt > DATE('$start_date3') AND reports.gmt <=DATE('$end_date3') AND reports.ip NOT LIKE '%212.17.10.114%' AND users.ip  NOT LIKE '%212.17.10.114%' AND reports.ip != '95.170.150.101' AND reports.ip NOT LIKE '%192.168%' AND users.ip NOT LIKE '%192.168%';");
$query_users4 = mysqli_query($con, "select count(DISTINCT user_id) as sum from reports INNER JOIN users ON reports.user_id=users.id where reports.gmt > DATE('$start_date4') AND reports.gmt <=DATE('$end_date4') AND reports.ip NOT LIKE '%212.17.10.114%' AND users.ip  NOT LIKE '%212.17.10.114%' AND reports.ip != '95.170.150.101' AND reports.ip NOT LIKE '%192.168%' AND users.ip NOT LIKE '%192.168%';");

#$start_date=date_create($start_date)->format('m-d');
#$end_date=date_create($end_date)->format('m-d');
#$start_date2=date_create($start_date2)->format('m-d');
#$end_date2=date_create($end_date2)->format('m-d');
#$start_date3=date_create($start_date3)->format('m-d');
#$end_date3=date_create($end_date3)->format('m-d');
#$start_date4=date_create($start_date4)->format('m-d');
#$end_date4=date_create($end_date4)->format('m-d');


#запустим скрипт, который найдет лучшие результаты
#Идея такая - пускается скрипт find_best.php, он итерирует всевозможные результаты на 50 недель назад (подправьте число 50 в скрипте, если хотите лучше), затем пишет то, что нашел, в файл. Затем, вот этот скрипт, в котором мы находимся, читает значения из файла и сравнивает с текущими показателсями. Поскольку итерировать много раз, да еще и на каждый тик делать два запроса в базу - это долго, а значения в прошлом никак не меняются, то прогнав этот скрипт один раз я его просто отключил (значения уже записаны в best.txt. Если по каким-то причинам нам нужно проверить лучшее значение снова в другой день (например, изменился день собрания), то просто раскомментируйте эти строчки в новый день собрания (это важно, объяснять почему долго, читайте скрипт и поймете), один раз открываем страницу со статистикой (данные best.txt на этом моменте обновятся) и снова закомментируйте.
#include 'find_best.php';
#calculateBestValues($con);

#Возьмем из файла лучшие результаты
$file_handle = fopen("best.txt", "r");
$launches=fgets($file_handle);
$users=fgets($file_handle);
fclose($file_handle);
preg_match('/Maximum launches: ([0-9]+), from ([0-9]{4}-[0-9]{2}-[0-9]{2}) to ([0-9]{4}-[0-9]{2}-[0-9]{2})./', $launches, $matches_launchers);
preg_match('/Maximum users: ([0-9]+), from ([0-9]{4}-[0-9]{2}-[0-9]{2}) to ([0-9]{4}-[0-9]{2}-[0-9]{2})./', $users, $matches_users);
$max_launches = $matches_launchers[1];
$launches_start_week = $matches_launchers[2];
$launches_end_week =  $matches_launchers[3];

$max_users = $matches_users[1];
$users_start_week =  $matches_users[2];
$users_end_week =  $matches_users[3];


$rows3 = array();


//reports
$best = 'log.txt';
unlink($best);
$log = fopen($best, "a+");

$row = mysqli_fetch_row ($query_users);
fprintf($log, $row);
$row2 = mysqli_fetch_row ($query_users2);
$row3 = mysqli_fetch_row ($query_users3);
$row4 = mysqli_fetch_row ($query_users4);


#надо переписать файл, если значения лучше
$rewrite = false;
$new_users = "";
if ($max_users < $row2[0]) {
    $current_year = date ( 'Y' );
    $new_users = "Maximum users: ${row2[0]}, from ${current_year}-${start_date} to ${current_year}-${end_date}.\n";
    $max_users = $row2[0];
    $users_start_week = "${current_year}-${start_date}";
    $users_end_week =  "${current_year}-${end_date}";
    $rewrite = true;
} else {
    $new_users = $users;
}

//reports
$temp = array();
$temp[]="Кол-во пользователей";
$temp[]=$row[0];
$temp[]=$row2[0];
$temp[]=$row3[0];
$temp[]=$row4[0];
$temp[]=$max_users;
$temp[]='-';
$rows3[]=$temp;

//ugeneui launch
$row = mysqli_fetch_row ($query);
$row2 = mysqli_fetch_row ($query2);
$row3 = mysqli_fetch_row ($query3);
$row4 = mysqli_fetch_row ($query4);

#надо переписать файл, если значения лучше
$new_launches = "";
if ($max_launches < $row2[1]) {
    $current_year = date ( 'Y' );
    $new_launches = "Maximum launches: ${row2[1]}, from ${current_year}-${start_date} to ${current_year}-${end_date}.\n";
    $max_launches = $row2[1];
    $launches_start_week = "${current_year}-${start_date}";
    $launches_end_week =  "${current_year}-${end_date}";
    $rewrite = true;
} else {
    $new_launches = $launches;
}

$temp = array();
$temp[]= "Кол-во запусков UGENE";
$temp[]=$row[1];
$temp[]=$row2[1];
$temp[]=$row3[1];
$temp[]=$row4[1];
$temp[]='-';
$temp[]=$max_launches;
$rows3[]=$temp;


if ($rewrite) {
    $best = 'best.txt';
    unlink($best);
    $fileWithLog = fopen($best, "a+");
    fprintf($fileWithLog, $new_launches);
    fprintf($fileWithLog, $new_users);
    fclose($file_handle);
}

function getArrow($next_w, $prev_w){
    if ($prev_w == $next_w){
	return "&#8594;";
    }else{
        return $prev_w < $next_w ? "<font color='green'>&#8599;</font>" : "<font color='red'>&#8600;</font>" ;
    }
}


$rows = array();
foreach($rows3 as $key => $val) {
    $temp = array();
    $counter=$val['0'];
    $temp[] = array('v' => $counter);
    $temp[] = array('v' => (int) $val['4']);
    $temp[] = array('v' => (int) $val['3']);
    $temp[] = array('v' => (int) $val['2']);
    $temp[] = array('v' => (int) $val['1']);
    $temp[] = array('v' => getArrow($val['3'],$val['4']).getArrow($val['2'],$val['3']).getArrow($val['1'],$val['2']) );
    $temp[] = array('v' => (string) $val['5']);
    $temp[] = array('v' => (string) $val['6']);

    $rows[] = array('c' => $temp);
}

$table = array();
$table['cols'] = array(
    array('label' => "Параметр", 'type' => 'string'),
    array('label' => '4 недели<br/>назад<br/>('.$start_date4.'&minus;<br/>'.$end_date4.')', 'type' => 'number'),
    array('label' => '3 недели<br/>назад<br/>('.$start_date3.'&minus;<br/>'.$end_date3.')', 'type' => 'number'),
    array('label' => '3 недели<br/>назад<br/>('.$start_date3.'&minus;<br/>'.$end_date3.')', 'type' => 'number'),
    array('label' => 'Текущая<br/>неделя<br/>('.$start_date.'&minus;<br/>'.$end_date.')', 'type' => 'number'),
    array('label' => 'Тренд', 'type' => 'string'),
    array('label' => 'Лучшее<br/>'.$users_start_week.'<br/>'.$users_end_week, 'type' => 'string'),
    array('label' => 'Лучшее<br/>'.$launches_start_week.'<br/>'.$launches_end_week, 'type' => 'string')
);

// populate the table with rows of data
$table['rows'] = $rows;

// encode the table as JSON
$jsonTable = json_encode($table);

// set up header; first two prevent IE from caching queries
header('Cache-Control: no-cache, must-revalidate');
header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Content-type: application/json');

// return the JSON data
echo $jsonTable;
mysqli_close($con);
?>