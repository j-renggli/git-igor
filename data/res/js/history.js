setup_app = function(app) {
    app.controller("log_controller", log_controller);
};

var log_controller = function($scope) {

    $scope.commits = [
];

    $scope.addCommit = function(commit) {
	$scope.commits.push(commit);

        if ($scope.commits.length == 1)
	    $scope.lanes[0] = $scope.commits[0].id;

	$scope.updateLane(commit, $scope.lanes);

	$scope.$apply();
    }

    $scope.lanes = [];

    $scope.updateLane = function(commit, lanes) {
	commit.lane = lanes.indexOf(commit.id);
	var hasNext = false;
	for (var i in commit.parents) {
	    var parent = commit.parents[i];
	    var idx = lanes.indexOf(parent);
	    if (idx >= 0) continue;
	    if (!hasNext) {
		lanes[commit.lane] = parent;
		hasNext = true;
	    }
	    var next = lanes.indexOf(null);
	    if (next < 0)
		lanes.push(parent);
	    else
		lanes[next] = parent;
	}
    }

    $scope.updateGraph = function() {
	for (var i in $scope.commits) {
	    $scope.updateLane($scope.commits[i], $scope.lanes);
	}
    }

    $scope.updateGraph();

};

