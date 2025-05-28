
const char *htmlHomePage PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en"> <!-- Định nghĩa ngôn ngữ trang là tiếng Anh -->
<head>
    <meta charset="UTF-8"> <!-- Định dạng ký tự UTF-8 -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Responsive: khớp chiều rộng thiết bị -->
    <title>Clean Solar Panel Robot</title> <!-- Tiêu đề trang -->
    <style>
        /* ==== CSS định dạng giao diện ==== */

        html{
            font-family: Arial, Helvetica, sans-serif; /* Font chữ sử dụng */
            text-align: center; /* Canh giữa toàn bộ nội dung */
            display: inline-block; /* Hiển thị inline-block */
        }

        body{margin: 0;} /* Bỏ margin mặc định của body */

        /* Định nghĩa màu sắc và kích thước chữ cho span, p với trạng thái on/off */
        span, .span-on{
            color: rgb(242, 251, 244);
            /* màu trắng nhạt */
            font-size: 12px;
        }
        .span-off{
            color: rgb(9, 135, 34);
            /* màu xanh đậm */
            font-size: 12px;
        }
        .PeriphColor{
            color: #1472f6; /* màu xanh dương */
        }

        p, .p-on{
            color: rgb(242, 251, 244);
            font-size: 14px;
            font-weight: bold;
            margin-top: 2px;
            display: inline-block;
        }
        .p-off{
            color: rgb(9, 135, 34);
            font-size: 14px;
            font-weight: bold;
            margin-top: 2px;
            display: inline-block;
        }

        /* Định dạng nút bấm chung */
        button{
            text-align: center;
            margin: 2.4px 2.4px;
            margin-right: 4px;
            margin-left: 4px;
            border-radius: 7px; /* Bo góc */
            width: 56px;
            height: 25px;
            font-size: 12px;
            font-weight: bold;
            cursor:pointer; /* con trỏ khi hover */
        }

        /* Các class button1, button2, button3 là các kiểu nút với kích thước khác nhau */
        .button1{
            width: 45px;
            height: 20px;
            font-size: 18px;
            font-weight: 200;
        }
        .button2{
            width: 45px;
            height: 20px;
            font-size: 18px;
            font-weight: 200;
        }
        .button3{
            width: 30px;
            height: 20px;
            font-size: 18px;
        }

        /* Hiệu ứng khi hover chuột trên button - màu xanh nhạt */
        button:hover{   
            background-color: rgb(180, 212, 223);
        }
        /* Hiệu ứng khi click button - màu đỏ */
        button:active{  
            background-color:rgb(196, 30, 30);
        }

        /* Container tổng bao quanh toàn bộ giao diện */
        .container{
            padding: 3px;
            text-align: center;
            background-color: rgba(255,255,255,0); /* trong suốt */
            position: relative;
        }

        /* Thanh điều hướng (Navigation) */
        .Navigation{
            background-color: #f2f6f7; /* nền trắng xanh nhạt */
            display: inline-block;
            border-radius: 5px;
            width: 380px;
            height: 460px;
            flex: 1;
        }

        /* Khu vực thiết lập tham số */
        .Setup_Parameter{
            display: inline-block;
            width: 300px;
            height: auto;
            text-align: center;
            margin-bottom: 6px;
        }

        /* Các khối chức năng Manual, Auto, Brush_Control, Sensor đều có margin riêng biệt */
        .Manual{ margin-bottom: 5px; margin-top: 0.5px;} 
        .Auto{ margin-top: 0.5px; padding-bottom: 5px;}  
        .Brush_Control{ margin-bottom: 0.5px; margin-top: 10px;}
        .Sensor{ margin-bottom: 15px; margin-top: 0.5px;}

        /* Box chứa input kiểu inline-block */
        .input_box{
            display: inline-block;
            position: relative;
        }
        .input_box input{
            padding: 5px;
            text-align: center;
            font-size: 12px;
            font-weight: bold;
            border: 1px solid #ccc;
            border-radius: 5px;
            width: 50px;
            height: 10px;
        }
        .input_box input:focus{
            outline: none;
            border-color: rgb(74, 156, 214); /* viền xanh khi focus */
        }

        /* Box input kiểu khác với kích thước to hơn */
        .input_box2{
            display: inline-block;
            position: relative;
        }
        .input_box2 input{
            padding: 3px;
            text-align: center;
            font-size: 12px;
            font-weight: bold;
            border: 1px solid #ccc;
            border-radius: 5px;
            width: 65px;
            height: 14px;
        }
        .input_box2 input:focus{
            outline: none;
            border-color: rgb(74, 156, 214);
        }

        /* Định dạng bảng */
        table,th,td{
            border-collapse: collapse; /* Gộp viền */
            text-align: center; /* Căn giữa chữ */
            height: 0.3cm;  
        }
        td{
            width: 0.05cm;
        }
        th{
            width: 1.65cm;
            font-size: 12px;
            color: rgb(242, 251, 244); /* Màu trắng nhạt */
        }

        /* Định nghĩa Toggle Switch (công tắc bật tắt kiểu chuyển slider) */
        .toggle {
            position: relative;
            display: inline-block;
            width: 50px;
            height: 24px;
        }

        /* Ẩn checkbox gốc */
        .toggle input[type="checkbox"] {
            display:none;
            opacity: 0;
            width: 0;
            height: 0;
        }

        /* Nền thanh trượt */
        .toggle-slider {
            position: absolute;
            cursor: pointer;
            top: 0; left: 0; right: 0; bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 30px;
        }

        /* Nút tròn trượt */
        .toggle-slider:before {
            position: absolute;
            content: "";
            height: 18px;
            width: 18px;
            left: 4px;
            bottom: 3px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }

        /* Khi checkbox checked, đổi màu nền thanh trượt */
        input[type="checkbox"]:checked + .toggle-slider {
            background-color: #2196F3; /* Xanh dương */
        }

        /* Khi focus */
        input[type="checkbox"]:focus + .toggle-slider {
            box-shadow: 0 0 1px #2196F3;
        }

        /* Nút tròn trượt sang bên phải khi checked */
        input[type="checkbox"]:checked + .toggle-slider:before {
            transform: translateX(26px);
        }

        /* Chữ OFF trên thanh trượt khi chưa bật */
        .toggle-slider:after {
            content:'OFF';
            color: white;
            display: block;
            position: absolute;
            transform: translate(-50%,-50%);
            top: 50%;
            left: 70%;
            font-size: 10px;
            font-family: Arial, Helvetica, sans-serif;
        }

        /* Chữ ON khi bật */
        input[type="checkbox"]:checked + .toggle-slider:after {  
            left: 35%;
            content:'ON';
        }

        /* Khi checkbox bị disable */
        input[type="checkbox"]:disabled + .toggle-slider {  
            opacity: 0.3;
            cursor: not-allowed;
            pointer-events: none;
        }

        /* Định dạng chữ, nhãn label trạng thái ON/OFF */
        .shape{
            font-size: 20px;
            color: rgb(32, 176, 233);
            text-align: center;
        }
        label, .label-on{
            margin-top: 5px;
            font-size: 13px;
            font-weight: bold;
            display: inline-block;
            color: rgb(242, 251, 244);
        }
        .label-off{
            margin-top: 5px;
            font-size: 13px;
            font-weight: bold;
            display: inline-block;
            color: rgb(9, 135, 34);
        }

        /* Đèn LED báo trạng thái */
        .led{
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background-color: rgb(119, 241, 119); /* màu xanh lá */
            display: inline-block;
            margin: 10px;
        }

        /* Định vị hình ảnh/video camera đặt ở giữa, nằm dưới */
        img.stream, video.stream{
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            z-index: -1; /* Nằm dưới các thành phần khác */
        }

        /* Thẻ card chứa các khu vực nhỏ */
        .card{
            background-color: rgba(255,255,255,0);
            box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5); 
            border: 1.3px solid #00878f; 
            border-radius: 3.5px;
            height: 320px;
            z-index: 0;
            position: relative;
        }

        /* Hai thẻ card bên trong container: sử dụng grid để tự động căn chỉnh */
        .twocard {
            max-width: 620px; 
            max-height: 320px;
            margin: 0 auto; 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
        } 

        /* Hộp hiển thị phần pin ở góc trên phải */
        .battery-container {
          display: flex;
          align-items: center;
          font-size: 20px;
          position: absolute;
          top: 10px; /* cách mép trên 10px */
          right: 10px; /* cách mép phải 10px */
        }

        /* Biểu tượng pin (khung ngoài) */
        .battery-icon {
          width: 24px;
          height: 12px;
          border: 1px solid #ccc;
          border-radius: 3px;
          position: relative;
          margin-right: 5px;
        }

        /* Mức pin bên trong (điền màu xanh tùy % pin) */
        .battery-level {
          position: absolute;
          top: 1px;
          left: 1px;
          bottom: 1px;
          width: 80%; /* chiều rộng động thay đổi theo % */
          background-color: #4CAF50; /* màu xanh lá */
          border-radius: 2px;
        }

        /* Phần % pin bên cạnh */
        .battery-percentage {
          font-size: 15px;
          font-weight: bold;
        }

        /* Animation keyframe cho tooltip (chưa sử dụng trong HTML) */
        @-webkit-keyframes tooltipkeys{ 0% {opacity: 0;} 75% {opacity: 0;} 100% {opacity: 1;}}
        @-moz-keyframes tooltipkeys{ 0% {opacity: 0;} 75% {opacity: 0;} 100% {opacity: 1;}}
        @-o-keyframes tooltipkeys{ 0% {opacity: 0;} 75% {opacity: 0;} 100% {opacity: 1;}}
        @keyframes tooltipkeys{ 0% {opacity: 0;} 75% {opacity: 0;} 100% {opacity: 1;}}
    </style>
</head>

<body>
    <div class="container">
        <!-- Ảnh hoặc video luồng camera -->
        <img class="stream" id="cameraImg" width="620px" height="320px">

        <!-- Hai card chính hiển thị thông tin và điều khiển -->
        <div class="twocard">

            <!-- Card điều khiển -->
            <div class="card">
                <div class="Setup_Parameter">
                    <!-- Tiêu đề khu vực điều khiển tốc độ -->
                    <p class="p-off">SPEED CONTROL</p>

                    <!-- Bảng điều chỉnh tốc độ -->
                    <table>
                        <tr>
                            <th></th><th></th>
                            <th><label for="setpoint1" class="label-off">Motor</label></th>
                            <td><button class="button1 decrease1">-</button></td>
                            <td>
                                <div class="input_box">
                                    <!-- Input tốc độ motor (wheelspeed) -->
                                    <input type="number" id="wheelspeed" name="wheelspeed" min="0" max="23" value="%wheelspeed%" required>
                                </div>
                            </td>
                            <td><button class="button1 increase1">+</button></td>
                            <th></th><th></th>
                        </tr>
                        <tr>
                            <td></td><td></td>
                            <th><label for="setpoint2" class="label-off">Brush</label></th>
                            <td><button class="button2 decrease2">-</button></td>
                            <td>
                                <div class="input_box">
                                    <!-- Input tốc độ bàn chải -->
                                    <input type="number" id="brushspeed" name="brushspeed" min="0" max="53" value="%brushspeed%" required>
                                </div>
                            </td>
                            <td><button class="button2 increase2">+</button></td> 
                        </tr>
                    </table>

                    <!-- Nút gửi thiết lập -->
                    <div class="button button-setup">
                        <button onclick="sendParameter('setup')">Setup</button>
                    </div>

                    <!-- Bảng hiển thị vận tốc và khoảng cách -->
                    <table>
                        <tr>
                            <th></th>
                            <th><label for="velocity" class="label-off">Velocity</label></th>
                            <td>
                                <div class="input_box">
                                    <!-- Input vận tốc, readonly -->
                                    <input type="number" id="velocity" name="velocity" min="0" max="1" readonly>
                                </div>
                            </td>
                            <th></th>
                            <th><label for="move" class="label-off">Space</label></th>
                            <td>
                                <div class="input_box">
                                    <!-- Input khoảng cách di chuyển, readonly -->
                                    <input type="number" id="move" name="move" min="0" max="1000" readonly>
                                </div>
                            </td>
                            <th></th>
                        </tr>
                    </table>
                </div>
                <br><br>

                <!-- Phần điều khiển hướng đi (Manual) -->
                <div class="Manual">
                    <p class="p-off">DIRECTION</p>
                    <table>
                        <!-- Hàng trên cùng: đèn led 1, nút tiến (up), đèn led 2 -->
                        <tr>
                            <th></th><th></th><th></th>
                            <td><div class="led" id="led1"></div></td>
                            <td>
                                <div class="button button-forward">
                                    <!-- Nút bấm điều khiển tiến: bắt đầu khi nhấn, dừng khi nhả -->
                                    <button class="direction_motor" 
                                        onmousedown="sendDirection('forward')" 
                                        ontouchstart="sendDirection('forward')" 
                                        onmouseup="sendDirection('stop1')" 
                                        ontouchend="sendDirection('stop1')">
                                        <span class="shape">&#8679;</span> <!-- Mũi tên lên -->
                                    </button>
                                </div>
                            </td>
                            <td><div class="led" id="led2"></div></td>
                            <th></th><th></th>
                        </tr>

                        <!-- Hàng giữa: nút trái, nút dừng, nút phải -->
                        <tr>
                            <th></th><th></th><th></th>
                            <td>
                                <div class="button button-left" style="display:inline-block; text-align: left;">
                                    <!-- Nút trái -->
                                    <button class="direction_motor" 
                                        onmousedown="sendDirection('left')" 
                                        ontouchstart="sendDirection('left')" 
                                        onmouseup="sendDirection('stop1')" 
                                        ontouchend="sendDirection('stop1')">
                                        <span class="shape">&#8678;</span> <!-- Mũi tên trái -->
                                    </button>
                                </div>
                            </td>
                            <td>
                                <div class="button button-stop1" style="display:inline-block; text-align: center;">
                                    <!-- Nút dừng khẩn cấp -->
                                    <button onclick="sendDirection('stopEmergency')">Stop</button>
                                </div>
                            </td>
                            <td>
                                <div class="button button-right" style="display:inline-block; text-align: right;">
                                    <!-- Nút phải -->
                                    <button class="direction_motor" 
                                        onmousedown="sendDirection('right')" 
                                        ontouchstart="sendDirection('right')" 
                                        onmouseup="sendDirection('stop1')" 
                                        ontouchend="sendDirection('stop1')">
                                        <span class="shape">&#8680;</span> <!-- Mũi tên phải -->
                                    </button>
                                </div>
                            </td>
                            <th></th><th></th>
                        </tr>

                        <!-- Hàng dưới cùng: đèn led 3, nút lùi, đèn led 4 -->
                        <tr>
                            <th></th><th></th><th></th>
                            <td><div class="led" id="led3"></div></td>
                            <td>
                                <div class="button button-backward">
                                    <!-- Nút lùi -->
                                    <button class="direction_motor" 
                                        onmousedown="sendDirection('backward')" 
                                        ontouchstart="sendDirection('backward')" 
                                        onmouseup="sendDirection('stop1')" 
                                        ontouchend="sendDirection('stop1')">
                                        <span class="shape">&#8681;</span> <!-- Mũi tên xuống -->
                                    </button>
                                </div>
                            </td>
                            <td><div class="led" id="led4"></div></td>
                            <th></th><th></th>
                        </tr>
                    </table>
                </div>
            </div>

            <!-- Card trạng thái các thiết bị và bật tắt -->
            <div class="card">
                <div style="position:relative">
                    <!-- Hiển thị pin ở góc phải trên -->
                    <div class="battery-container">
                        <div class="battery-icon">
                          <div class="battery-level"></div> <!-- Mức pin động -->
                        </div>
                        <div class="battery-percentage">99%</div> <!-- % pin -->
                    </div>
                    <br>

                    <!-- Toggle Brush (bàn chải) -->
                    <div style="position:relative; float: left; top: 5px; width: 42%; left: 0px; text-align: right;">
                        <span class="PeriphColor" style="font-weight:bold;">BRUSH</span> 
                    </div>
                    <div style="position:relative; float: right; width: 52%; right: 0px;">
                        <div style="position:relative; float: left;">
                            <label class="toggle">
                                <input type="checkbox" id="status-brush" onchange="toggleBrush(this)">
                                <span class="toggle-slider"></span> <!-- Thanh trượt -->
                            </label>
                        </div>
                    </div>

                    <!-- Toggle Pump (bơm nước) -->
                    <div style="position:relative; float: left; top: 22px; width: 42%; left: 0px; text-align: right;">
                        <span class="PeriphColor" style="font-weight:bold;">PUMP </span>
                    </div>
                    <div style="position:relative; float: right; width: 52%; right: 0px; top: 5px;">
                        <div style="position:relative; float: left;">
                            <label class="toggle">
                              <input type="checkbox" id="status-pump" onchange="togglePump(this)">
                              <div class="toggle-slider"></div>
                            </label>
                        </div>
                    </div>

                    <!-- Toggle Auto Mode -->
                    <div style="position:relative; float: left; top: 44px; width: 42%; left: 0px; text-align: right;">
                        <span class="PeriphColor" style="font-weight:bold;">AUTO MODE</span>
                    </div>
                    <div style="position:relative; float: right; width: 52%; right: 0px; top: 10px;">
                        <div style="position:relative; float: left;">
                            <label class="toggle">
                            <input type="checkbox" id="status-auto" onchange="toggleAutoMode(this)">
                            <div class="toggle-slider"></div>
                            </label>
                        </div>
                    </div>

                    <!-- Toggle Camera -->
                    <div style="position:relative; float: left; top: 50px; width: 42%; left: 0px; text-align: right;">
                        <span class="PeriphColor" style="font-weight:bold;">CAMERA</span>
                    </div>
                    <div style="position:relative; float: right; width: 52%; right: 0px; top: 15px;">
                        <div style="position:relative; float: left;">
                            <label class="toggle">
                            <input type="checkbox" id="status-camera" onchange="toggleCamera(this)">
                            <div class="toggle-slider"></div>
                            </label>
                        </div>
                    </div>
                </div>
                <br><br><br><br><br><br><br><br><br>
                <div></div>

                <!-- Bảng hiển thị dữ liệu sensor -->
                <div class="Sensor">
                    <table>
                        <tr>
                            <th></th>
                            <th></th>
                            <th><p class="p-off" for="sensor1">Sensor1</p></th>
                            <td>
                                <div class="input_box2">
                                    <input type="number" id="H1" name="data1" min="0" max="100" readonly>
                                </div>
                            </td>
                            <td>
                                <div class="input_box2">
                                    <input type="number" id="H2" name="data2" min="0" max="100" readonly>
                                </div>
                            </td>
                            <th><p class="p-off" for="sensor2">Sensor2</p></th>
                            <th></th>
                            <th></th>
                        </tr>
                        <tr>
                            <th></th>
                            <th></th>
                            <th><p class="p-off" for="sensor3">Sensor3</p></th>
                            <td>
                                <div class="input_box2">
                                    <input type="number" id="H3" name="data3" min="0" max="100" readonly>
                                </div>
                            </td>
                            <td>
                                <div class="input_box2">
                                    <input type="number" id="H4" name="data4" min="0" max="100" readonly>
                                </div>
                            </td>
                            <th><p class="p-off" for="sensor4">Sensor4</p></th>
                            <th></th>
                        </tr>
                    </table>
                    <!-- Nút nhận dữ liệu sensor -->
                    <span class="button button-receive">
                        <button onclick="startData()">Data</button>
                    </span>
                </div>
            </div>
        </div>
    </div>

    <script>
        // Hàm gửi lệnh điều hướng cho robot
        function sendDirection(direction){
            event.preventDefault(); // Ngăn chặn hành vi mặc định của event
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/" + direction, true); // Gửi GET request với path = /direction
            xhttp.send();
        }

        // Hàm gửi thông số speed (setup) cho server
        function sendParameter(command){
            var wheelspeed = document.getElementById("wheelspeed").value;
            var brushspeed = document.getElementById("brushspeed").value;
            
            // Kiểm tra giá trị hợp lệ
            if (wheelspeed === "" || brushspeed === "" || wheelspeed < 0 || wheelspeed > 23 || brushspeed < 0 || brushspeed > 53){
                alert("Please enter again data");
                return;
            }

            // Tạo URL gửi đến server với các tham số
            var url = "/" + command + "?wheelSpeed=" + wheelspeed + "&brushSpeed=" + brushspeed +
                                    "&statusBrush=0" + "&statusWheel=0" + "&statusAuto=0" + "&statusPump=0";
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", url, true);
            xhttp.send();
        }

        // Lấy các button và input để thao tác tăng giảm tốc độ
        const decreaseButton1 = document.querySelector('.decrease1');
        const increaseButton1 = document.querySelector('.increase1');
        const decreaseButton2 = document.querySelector('.decrease2');
        const increaseButton2 = document.querySelector('.increase2');
        const wheelSpeedInput = document.querySelector('#wheelspeed');
        const brushSpeedInput = document.querySelector('#brushspeed');

        // Gán sự kiện click cho các nút tăng giảm tốc độ motor và brush
        decreaseButton1.addEventListener('click', () => {changeSpeed1(-1);})
        increaseButton1.addEventListener('click', () => {changeSpeed1(+1);})
        decreaseButton2.addEventListener('click', () => {changeSpeed2(-1);})
        increaseButton2.addEventListener('click', () => {changeSpeed2(+1);})

        // Sự kiện nhấn Enter trong input tốc độ motor
        wheelSpeedInput.addEventListener('keyup', (event) => {
            if (event.key === 'Enter'){
                event.preventDefault();
                const newSpeed = parseInt(wheelSpeedInput.value);
                if ((!isNaN(newSpeed)) && (newSpeed >= 0) && (newSpeed <= 23)) {sendWheelSpeed(newSpeed);}
                else {console.error('Invalid speed value!');}
                wheelSpeedInput.blur();
            }
        });

        // Sự kiện nhấn Enter trong input tốc độ brush
        brushSpeedInput.addEventListener('keyup', (event) => {
            if (event.key === 'Enter'){
                event.preventDefault();
                const newSpeed = parseInt(brushSpeedInput.value);
                if ((!isNaN(newSpeed)) && (newSpeed >= 0) && (newSpeed <= 53)) {sendBrushSpeed(newSpeed);}
                else {console.error('Invalid speed value!');}
                brushSpeedInput.blur();
            }
        });

        // Hàm tăng giảm tốc độ motor theo delta (-1 hoặc +1)
        function changeSpeed1(delta){
            const currentSpeed = parseInt(wheelSpeedInput.value) || 0;
            let newSpeed = currentSpeed + delta;
            if (newSpeed < 0) newSpeed = 0;
            else if (newSpeed > 23) newSpeed = 23;
            wheelSpeedInput.value = newSpeed;
            sendWheelSpeed(newSpeed);
        }

        // Hàm tăng giảm tốc độ brush theo delta
        function changeSpeed2(delta){
            const currentSpeed = parseInt(brushSpeedInput.value) || 0;
            let newSpeed = currentSpeed + delta;
            if (newSpeed < 0) newSpeed = 0;
            else if (newSpeed > 53) newSpeed = 53;
            brushSpeedInput.value = newSpeed;
            sendBrushSpeed(newSpeed);
        }

        // Hàm gửi tốc độ motor lên server
        function sendWheelSpeed(speed){
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/changeSpeed1?value1=" + speed, true);
            xhttp.send();
        }

        // Hàm gửi tốc độ brush lên server
        function sendBrushSpeed(speed){
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/changeSpeed2?value2=" + speed, true);
            xhttp.send();
        }

        // Hàm xử lý giữ nút bấm (touch) điều khiển robot
        function touchStartHandle(){
            touchStartTimer = setTimeout(function(){sendDirection(direction);}, 400); 
            /* Nếu giữ hơn 400ms, robot sẽ chạy */
        }

        // Khi kết thúc chạm, gửi lệnh dừng
        function touchEndHandle(){
            clearTimeout(touchStartTimer);
            sendDirection('stop1');
        }

        // Bật/tắt Brush, gửi trạng thái qua server
        function toggleBrush(element){
            var xhr = new XMLHttpRequest();
            if (!element.checked){
                xhr.open("GET", "/brush?statusBrush=0", true);
            }
            else{
                xhr.open("GET", "/brush?statusBrush=1", true);
            }
            xhr.send();
        }

        // Biến tham chiếu tới phần tử ảnh camera
        const video = document.getElementById('cameraImg');
        const videoURL = 'http://192.168.222.172:5000/camera'; // URL luồng video camera

        // Bật/tắt camera, thay đổi URL ảnh, cập nhật màu sắc giao diện
        function toggleCamera(element){
            var statusCamera  = document.getElementById("status-camera");
            var labelElements = document.getElementsByTagName("label");
            var spanElements  = document.getElementsByTagName("span");
            var pElements     = document.getElementsByTagName("p");
            var xhr = new XMLHttpRequest();

            if (!element.checked){
                // Nếu tắt camera, thay đổi class để đổi màu chữ về trạng thái OFF
                for (var i=0; i < spanElements.length; i++){
                    spanElements[i].classList.remove("span-on");
                    spanElements[i].classList.add("PeriphColor");
                }
                for (var i=0; i < pElements.length; i++){
                    pElements[i].classList.remove("p-on");
                    pElements[i].classList.add("p-off");
                }
                for (var i=0; i < labelElements.length; i++){
                    labelElements[i].classList.remove("label-on");
                    labelElements[i].classList.add("label-off");
                }

                video.src = ''; // Xóa URL video (tắt video)
            }
            else{
                // Nếu bật camera, đổi màu chữ sang ON
                for (var i=0; i<spanElements.length; i++){
                    spanElements[i].classList.remove("PeriphColor");
                    spanElements[i].classList.add("span-on");
                }
                for (var i=0; i<pElements.length; i++){
                    pElements[i].classList.remove("p-off");
                    pElements[i].classList.add("p-on");
                }
                for (var i=0; i < labelElements.length; i++){
                    labelElements[i].classList.remove("label-off");
                    labelElements[i].classList.add("label-on");
                }

                video.src = videoURL; // Gán URL luồng video để hiển thị
                video.onerror = function(){
                    console.error('Cannot download video') // Log lỗi khi không lấy được video
                };
            }
        }

        // Bật/tắt chế độ tự động
        function toggleAutoMode(element){
            var xhr = new XMLHttpRequest();
            if (!element.checked){
                xhr.open("GET", "/auto?statusAuto=0", true);
            }
            else{
                xhr.open("GET", "/auto?statusAuto=1", true);
            }
            xhr.send();
        }

        // Bật/tắt bơm nước
        function togglePump(element){
            var xhr = new XMLHttpRequest();
            if (!element.checked){
                xhr.open("GET", "/pump?statusPump=0", true);
            }
            else{
                xhr.open("GET", "/pump?statusPump=1", true);
            }
            xhr.send();
        }

        /* 
            Dữ liệu nhận được server gửi về có dạng: 
            D_094_00000_0.000_00.00_00.00_00.00_00.00_00.00_0_0_0_0_\r\n
            Các giá trị sau _ là các thông số lần lượt (percent pin, velocity, sensor, led ...)
        */
        
        var percent = ""; // Biến lưu phần trăm pin hiện tại
        function getData(){
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/data", true);
            xhttp.onreadystatechange = function(){
                if (this.readyState === 4 && this.status === 200){
                    const values = this.responseText.trim().split('_'); // Tách dữ liệu nhận được theo dấu _
                    percent = parseInt(values[1]); // Lấy % pin từ giá trị thứ 2

                    // Xử lý giá trị velocity
                    if (values[3] === '0.000'){
                      document.getElementById('velocity').value = 0;
                    }
                    else{
                      document.getElementById('velocity').value = values[3];
                    }
                    
                    // Cập nhật khoảng cách move
                    document.getElementById('move').value = values[4];

                    // Cập nhật dữ liệu các sensor H1..H4
                    document.getElementById('H1').value = values[5];
                    document.getElementById('H2').value = values[6];
                    document.getElementById('H3').value = values[7];
                    document.getElementById('H4').value = values[8];

                    // Cập nhật trạng thái LED dựa trên các giá trị flag
                    if (CheckLed(values[9])) toggleLed(0,true);
                    else toggleLed(0,false);

                    if (CheckLed(values[10])) toggleLed(1,true);
                    else toggleLed(1,false);

                    if (CheckLed(values[11])) toggleLed(2,true);
                    else toggleLed(2,false);

                    if (CheckLed(values[12])) toggleLed(3,true);
                    else toggleLed(3,false);
                }
                else{
                    console.log("Error receive data. Status code: " + xhttp.status);
                }
            };
            
            xhttp.send();
        }
        
        // Mảng trạng thái đèn led
        var ledStates = [false,false,false,false];

        // Hàm bật/tắt led tại vị trí index
        function toggleLed(index,state){
            var ledElement = document.getElementById('led' + (index+1));
            ledStates[index] = state;
            ledElement.style.backgroundColor = state ? 'red' : 'rgb(119, 241, 119)';
        }
        
        // Kiểm tra trạng thái led (0 hoặc 1)
        function CheckLed(flag){
            if (flag === 0)      return false;
            else if (flag === 1) return true;
        }

        var intervalID;

        // Bắt đầu lấy dữ liệu định kỳ mỗi 2 giây
        function startData(){
            intervalID = setInterval(getData,2000);
        }

        // Hàm bật tất cả đèn led
        function turnOn(){
            for (var i=0; i<4; i++){
                toggleLed(i,true);
            }
        }

        // Hàm tắt tất cả đèn led
        function turnOff(){
            for (var i=0; i<4; i++){
                toggleLed(i,false);
            }
        }

        // Cập nhật giao diện trạng thái pin
        function updateBatteryStatus() {
          // Nếu percent chưa có giá trị, đặt về 0%
          if (percent === "" || percent === 0){
              document.querySelector('.battery-percentage').textContent = 0 + '%';
          }
          else{
              document.querySelector('.battery-percentage').textContent = percent + '%';
          }

          // Cập nhật chiều rộng thanh pin theo phần trăm
          var batteryLevel = document.querySelector('.battery-level');
          batteryLevel.style.width = percent*1.0 + '%';

          // Đổi màu thanh pin theo mức % pin
          if (percent >= 80) {
              batteryLevel.style.backgroundColor = '#4CAF50'; // Xanh lá cây
          } 
          else if (percent >= 60) {
              batteryLevel.style.backgroundColor = '#FFA500'; // Cam
          } 
          else {
              batteryLevel.style.backgroundColor = '#FF0000'; // Đỏ
              alert("Low Voltage"); // Cảnh báo pin yếu
          }
        }

        // Cập nhật trạng thái Brush toggle (có check hay không)
        function updateStatusBrush(isChecked){
          var toggleInput = document.getElementById('status-brush');
          toggleInput.checked = isChecked;
        }
        // Cập nhật trạng thái Pump toggle
        function updateStatusPump(isChecked){
          var toggleInput = document.getElementById('status-pump');
          toggleInput.checked = isChecked;
        }
        // Cập nhật trạng thái Auto Mode toggle
        function updateStatusAuto(isChecked){
          var toggleInput = document.getElementById('status-auto');
          toggleInput.checked = isChecked;
        }

        // Biến websocket toàn cục
        var ws;

        // Khởi tạo websocket kết nối server
        function initWebSocket() {
          ws = new WebSocket("ws://" + window.location.hostname + "/ws");
          // Khi nhận dữ liệu từ server qua ws
          ws.onmessage = function(event) {
            var data = JSON.parse(event.data); // Parse dữ liệu JSON
            // Cập nhật các giá trị lên giao diện
            document.getElementById("wheelspeed").value = data.wheelSpeed;
            document.getElementById("brushspeed").value = data.brushSpeed;
            if (data.statusBrush === 1 ) updateStatusBrush(true);
            else updateStatusBrush(false);
            if (data.statusPump === 1 ) updateStatusPump(true);
            else updateStatusPump(false);
            if (data.statusAuto === 1 ) updateStatusAuto(true);
            else updateStatusAuto(false);
          };
        }
    
        // Khởi tạo khi tải trang xong
        window.onload = function() {
          initWebSocket();
        };
    </script>
</body>
</html>
)rawliteral";