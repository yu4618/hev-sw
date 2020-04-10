var chart;
var chart2;
var chart_new;
/**
 * Request data from the server, add it to the graph and set a timeout
 * to request again
 */
function requestDataVar1(var1, var2) {
    $.ajax({
        url: '/live-data',
        success: function(point) {

            if(chart.data.datasets[0].data.length > 60){
                chart.data.labels.shift();
                chart.data.datasets[0].data.shift();
            }

            if(chart.data.datasets[1].data.length > 60){
                //chart.data.labels.shift();
                chart.data.datasets[1].data.shift();
            }

            // add the point
            chart.data.labels.push(point.created_at);
            chart.data.datasets[0].data.push(point[var1]);
            chart.data.datasets[1].data.push(point[var2]);
            
            chart.update();
        },
        cache: false
    });
    // call it again after one second
    setTimeout(requestDataVar1, 1000, var1, var2);
}


function requestDataVar(var1, var2) {
    $.ajax({
        url: '/live-data',
        success: function(point) {

            if(chart_new.data.datasets[0].data.length > 60){
                chart_new.data.labels.shift();
                chart_new.data.datasets[0].data.shift();
            }

            if(chart_new.data.datasets[1].data.length > 60){
                //chart.data.labels.shift();
                chart_new.data.datasets[1].data.shift();
            }

            // add the point
            chart_new.data.labels.push(point.created_at);
            chart_new.data.datasets[0].data.push(point[var1]);
            chart_new.data.datasets[1].data.push(point[var2]);
            
            chart_new.update();
        },
        cache: false
    });
    // call it again after one second
    setTimeout(requestDataVar, 1000, var1, var2);
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

/*
function requestDataVar3() {
    $.ajax({
        url: '/live-data',
        success: function(point) {
            if(chart3.data.datasets[0].data.length > 20){
                chart3.data.labels.shift();
                chart3.data.datasets[0].data.shift();
            }

            // add the point
            chart3.data.labels.push(point.created_at);
            chart3.data.datasets[0].data.push(point.pressure);
            
            chart3.update();
        },
        cache: false
    });
    // call it again after one second
    setTimeout(requestDataVar3, 1000);
}
*/





$(document).ready(function() {
    var ctx = document.getElementById('pressure_air_supply');
    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{ 
      label: 'A',
      yAxisID: 'A',
                data: [],
                label: "pressure_air_supply",
                borderColor: "#3e95cd",
                fill: false,
              },{ 
      label: 'B',
      yAxisID: 'B',
                data: [],
                label: "B",
                borderColor: "#FFF000",
                fill: false,
              } 
            ]
          },
          options: {
            title: {
              display: false,
              text: 'pressure_air_supply'
            },
            scales: {
		xAxes: [{
                    type: 'time',
                    time: {
			unit: 'second',
			displayFormat: 'second'
                    },
		    ticks: {
			maxTicksLimit: 5,
			maxRotation: 0
		    }
		}],
                yAxes: [{
                   id: 'A',
                   type: 'linear',
                   position: 'left',
                }, {
                   id: 'B',
                   type: 'linear',
                   position: 'right',
                }]
	        },
		legend : {
		    display: false}
          }
    });
    requestDataVar1("temperature", "pressure");
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
                fill: false,
              } 
            ]
          },
          options: {
            title: {
              display: false,
              text: 'Variable 2'
            },
            scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'second',
                    displayFormat: 'second'
                },
		ticks: {
		    maxTicksLimit: 5,
		    maxRotation: 0
		}
            }]
            },
	legend : {
	    display: false
	},
	      layout : {
		  padding : {
		      left: 0,
		      right: 0,
		      top: 0,
		      bottom: 0

		  }
	      }
	  }
    });
    requestDataVar2();
});

/*

$(document).ready(function() {
    var ctx3 = document.getElementById('chart_variable3');
    chart3 = new Chart(ctx3, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{ 
                data: [],
                label: "Var3",
                borderColor: "#3e95cd",
                fill: false,
              } 
            ]
          },
          options: {
            title: {
              display: false,
              text: 'Variable 3'
            },
            scales: {
            xAxes: [{
                type: 'time',
                time: {
                    unit: 'second',
                    displayFormat: 'second'
                },
		ticks: {
		    maxTicksLimit: 5,
		    maxRotation: 0
		}
            }]
            },
	legend : {
	    display: false
	},
	      layout : {
		  padding : {
		      left: 0,
		      right: 0,
		      top: 0,
		      bottom: 0

		  }
	      }
	  }
    });
    requestDataVar3();
});

*/





// Function to parse the selection from a multiselect
function getSelectValues(select) {
  var result = [];
  var options = select && select.options;
  var opt;

  for (var i=0, iLen=options.length; i<iLen; i++) {
    opt = options[i];

    if (opt.selected) {
      result.push(opt.value || opt.text);
    }
  }
  return result;
}




// Function runs on chart type select update
function updateChartType() {
    // here we destroy/delete the old or previous chart and redraw it again
    chart.destroy();
/*
    chart = new Chart(ctx, {
       type: document.getElementById("chart_dataset").value,
       data: myData,
    });
*/
    var selection = document.multiselect('#chart_variables')._item
    var selection_results = getSelectValues(selection)
    console.log(selection_results[0]);

$(document).ready(function() {
    var ctx_new = document.getElementById('pressure_air_supply');
    chart_new = new Chart(ctx_new, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{ 
      label: 'A',
      yAxisID: 'A',
                data: [],
                label: "pressure_air_supply",
                borderColor: "#3e95cd",
                fill: false,
              },{ 
      label: 'B',
      yAxisID: 'B',
                data: [],
                label: "B",
                borderColor: "#FFF000",
                fill: false,
              } 
            ]
          },
          options: {
            title: {
              display: false,
              text: 'pressure_air_supply'
            },
            scales: {
		xAxes: [{
                    type: 'time',
                    time: {
			unit: 'second',
			displayFormat: 'second'
                    },
		    ticks: {
			maxTicksLimit: 5,
			maxRotation: 0
		    }
		}],
                yAxes: [{
                   id: 'A',
                   type: 'linear',
                   position: 'left',
                }, {
                   id: 'B',
                   type: 'linear',
                   position: 'right',
                }]
	        },
		legend : {
		    display: false}
          }
    });
    requestDataVar(selection_results[0], selection_results[1]);
});


}; // ends update button

    document.multiselect('#chart_variables')        
                .setCheckBoxClick("checkboxAll", function(target, args) {
                        console.log("Checkbox 'Select All' was clicked and got value ", args.checked);})
                .setCheckBoxClick("1", function(target, args) {
                        console.log("Checkbox for item with value '1' was clicked and got value ", args.checked); });



