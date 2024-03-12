// Assuming Chart.js is used

var ctx = document.getElementById('myChart').getContext('2d');
var chart = new Chart(ctx, {
    type: 'line', // Adjust chart type as needed
    data: {
        datasets: [{
            label: 'Serial Data',
            data: [], // Initially empty
            backgroundColor: 'rgba(255, 99, 132, 0.2)',
            borderColor: 'rgba(255, 99, 132, 1)',
            borderWidth: 1
        }]
    },
    options: {
        scales: {
            xAxes: [{
                time: { // Enable timestamps for x-axis (optional)
                    unit: 'second'
                }
            }]
        }
    }
});

function updateChart(data) {
    chart.data.datasets[0].data = data;
    chart.update();
}
