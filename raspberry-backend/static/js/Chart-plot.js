var chart;
var chart2;

/**
 * Request data from the server, add it to the graph and set a timeout
 * to request again
 */
function requestDataVar1() {
    $.ajax({
        url: '/live-data',
        success: function(point) {
            if(chart.data.datasets[0].data.length > 20){
                chart.data.labels.shift();
                chart.data.datasets[0].data.shift();
            }

            // add the point
            chart.data.labels.push(point.created_at);
            chart.data.datasets[0].data.push(point.temperature);
            
            chart.update();
        },
        cache: false
    });
    // call it again after one second
    setTimeout(requestDataVar1, 1000);
}


function requestDataVar2() {
    $.ajax({
        url: '/live-data',
        success: function(point) {
            if(chart2.data.datasets[0].data.length > 20){
                chart2.data.labels.shift();
                chart2.data.datasets[0].data.shift();
            }

            // add the point
            chart2.data.labels.push(point.created_at);
            chart2.data.datasets[0].data.push(point.pressure);
            
            chart2.update();
        },
        cache: false
    });
    // call it again after one second
    setTimeout(requestDataVar2, 1000);
}





$(document).ready(function() {
    var ctx = document.getElementById('variable1');
    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{ 
                data: [],
                label: "Var1",
                borderColor: "#3e95cd",
                fill: false
              } 
            ]
          },
          options: {
            title: {
              display: true,
              text: 'Variable 1'
            },
            scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'second',
                    displayFormat: 'second'
                }
            }]
        }
          }
    });
    requestDataVar1();
});




$(document).ready(function() {
    var ctx2 = document.getElementById('variable2');
    chart2 = new Chart(ctx2, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{ 
                data: [],
                label: "Var2",
                borderColor: "#3e95cd",
                fill: false
              } 
            ]
          },
          options: {
            title: {
              display: true,
              text: 'Variable 2'
            },
            scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'second',
                    displayFormat: 'second'
                }
            }]
        }
          }
    });
    requestDataVar2();
});


