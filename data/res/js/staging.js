setup_app = function(app) {
    app.controller("stage_controller", stage_controller);
};

var stage_controller = function($scope) {
  $scope.signs = {"add": "+", "del": "-"};

  $scope.mouseDown = function(context, index) {
    // Reset previous selection
    context.endSelected = null;
    context.beginSelected = index;
  };

  $scope.mouseUp = function(context, index) {
    context.endSelected = index+1;
    if (index < context.beginSelected) {
      context.endSelected = context.beginSelected;
      context.beginSelected = index;
    }
  };

  $scope.isSelected = function(context, index) {
    if (context.endSelected == null)
      return false;

    return (index >= context.beginSelected && index < context.endSelected);
  }

  $scope.stageHunk = function (context) {
    context.hidden = true;
    window.qtio.onStageHunk(context.id);
  }

  $scope.stageLines = function (context) {
    var hide = window.qtio.onStageLines(context.id, context.beginSelected, context.endSelected);
    context.hidden = hide;
  }

  $scope.selectedLines = function() {
    if (window.getSelection) {
      var sel = window.getSelection();
      if (sel.isCollapsed)
        return [];

      var range = sel.getRangeAt(0);

      var begin = range.startContainer;
      var end = range.startContainer;
      console.log(begin);
      console.log(end);
      if (begin == end)
        return [begin];
      var nodes = [];
      var node = begin;
      while (node && node != end) {
        nodes.push(node = nextNode(node));
      }

      node = begin;
      while (node && node != range.commonAncestorContainer) {
        nodes.unshift(node);
        node = node.parentNode;
      }
      return nodes;
    }
  }

  $scope.setFileInfo = function(fileInfo) {
    $scope.fileInfo = fileInfo;
  }

  $scope.setContexts = function(contexts) {
    $scope.contexts = contexts;
    for (var i in $scope.contexts) {
      $scope.contexts[i].discarding = false;
    }
  };

  $scope.contexts = [];

};

