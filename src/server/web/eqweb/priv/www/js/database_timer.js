$(document).everyTime(5000, 'timer', function() {
    database_status();
});

$(document).oneTime(1000, 'one_timer', function() {
    show_node_list();
});