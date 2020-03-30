var chart;

/**
 * Request data from the server, add it to the graph and set a timeout
 * to request again
 */
function requestDataVar1() {
    $.ajax({
        url: '/live-data',
        success: function(point) {
            var series = chart.series[0],
                shift = series.data.length > 20; // shift if the series is
                                                 // longer than 20

            // add the point
            chart.series[0].addPoint([point.created_at, point.temperature], true, shift);


            // call it again after one second
            setTimeout(requestDataVar1, 1000);
        },
        cache: false
    });
}


function requestDataVar2() {
    $.ajax({ 
        url: '/live-data',
        success: function(point) {
            var series = chart2.series[0], 
                shift = series.data.length > 20; // shift if the series is
                                                 // longer than 20

            // add the point
            chart2.series[0].addPoint([point.created_at, point.pressure], true, shift);

            // call it again after one second
            setTimeout(requestDataVar2, 1000);
        },
        cache: false
    });
}

function requestDataVar3() {
    $.ajax({ 
        url: '/live-data',
        success: function(point) {
            var series = chart3.series[0], 
                shift = series.data.length > 20; // shift if the series is
                                                 // longer than 20

            // add the point
            chart3.series[0].addPoint([point.created_at, point.temperature], true, shift);

            // call it again after one second
            setTimeout(requestDataVar3, 1000);
        },
        cache: false
    });
}





$(document).ready(function() {
    chart = new Highcharts.Chart({
        chart: {
            renderTo: 'data-container-var-1',
            defaultSeriesType: 'spline',
            events: {
                load: requestDataVar1
            }
        },
        title: {
            text: 'VARIABLE 1'
        },
        xAxis: {
            minPadding: 0.4,
            maxPadding: 0.4,
            title: {
                text: 'Time',
                margin: 20
            },
            type: 'datetime',
            tickPixelInterval: 150,
            maxZoom: 20 * 1000
        },
        yAxis: {
            minPadding: 0.2,
            maxPadding: 0.2,
            title: {
                text: 'Reading',
                margin: 80
            }
        },
        series: [{
            name: 'INPUT',
            data: []
        }]
    });
});







$(document).ready(function() {
    chart2 = new Highcharts.Chart({
        chart: {
            renderTo: 'data-container-var-2',
            defaultSeriesType: 'spline',
            events: {
                load: requestDataVar2
            }
        },
        title: {
            text: 'VARIABLE 2'
        },
        xAxis: {
            minPadding: 0.4,
            maxPadding: 0.4,
            title: {
                text: 'Time',
                margin: 20
            },
            type: 'datetime',
            tickPixelInterval: 150,
            maxZoom: 20 * 1000
        },
        yAxis: {
            minPadding: 0.2,
            maxPadding: 0.2,
            title: {
                text: 'Reading',
                margin: 80
            }
        },
        series: [{
            name: 'INPUT',
            data: []
        }]
    });
});


$(document).ready(function() {
    chart3 = new Highcharts.Chart({
        chart: {
            renderTo: 'data-container-var-3',
            defaultSeriesType: 'spline',
            events: {
                load: requestDataVar3
            }
        },
        title: {
            text: 'VARIABLE 3'
        },
        xAxis: {
            minPadding: 0.4,
            maxPadding: 0.4,
            title: {
                text: 'Time',
                margin: 20
            },
            type: 'datetime',
            tickPixelInterval: 150,
            maxZoom: 20 * 1000
        },
        yAxis: {
            minPadding: 0.2,
            maxPadding: 0.2,
            title: {
                text: 'Reading',
                margin: 80
            }
        },
        series: [{
            name: 'INPUT',
            data: []
        }]
    });
});


