#include "ikjoint.h"
#include "ccdikresolver.h"

void moveIkJoints(const JointNodeTree &inputJointNodeTree, JointNodeTree &outputJointNodeTree,
    int startJointIndex, int endJointIndex, QVector3D destination, std::map<int, JointConstraint> *constrants)
{
    CCDIKSolver ikSolver;
    ikSolver.setMaxRound(10);
    int loopIndex = endJointIndex;
    
    std::vector<int> ikSolvingIndicies;
    while (-1 != loopIndex) {
        const auto &joint = inputJointNodeTree.joints()[loopIndex];
        ikSolvingIndicies.push_back(loopIndex);
        if (loopIndex == startJointIndex)
            break;
        loopIndex = joint.parentIndex;
    }
    std::reverse(std::begin(ikSolvingIndicies), std::end(ikSolvingIndicies));
    for (const auto &jointIndex: ikSolvingIndicies) {
        const JointConstraint *constraint = nullptr;
        if (nullptr != constrants) {
            const auto &findResult = constrants->find(jointIndex);
            if (findResult != constrants->end())
                constraint = &findResult->second;
        }
        ikSolver.addNodeInOrder(inputJointNodeTree.joints()[jointIndex].position, constraint);
    }
    
    ikSolver.solveTo(destination);
    int nodeCount = ikSolver.getNodeCount();
    Q_ASSERT(nodeCount == (int)ikSolvingIndicies.size());
    
    for (int i = 0; i < nodeCount; i++) {
        int jointIndex = ikSolvingIndicies[i];
        const QVector3D &newPosition = ikSolver.getNodeSolvedPosition(i);
        outputJointNodeTree.joints()[jointIndex].position = newPosition;
    }
}
