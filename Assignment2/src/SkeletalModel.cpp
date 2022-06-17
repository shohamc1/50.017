///=========================================================================================///
///
///                       Functions to be filled in for Assignment 2
///
///           IMPORTANT: you ONLY need to work on functions with TODO in this section
///
///=========================================================================================///

#include "SkeletalModel.h"

///=========================================================================================///
///                                    Load .skel File
///=========================================================================================///

// TODO: Load the skeleton from file here, create hierarchy of joints
//       (i.e., set values for m_rootJoint and m_joints)
void SkeletalModel::loadSkeleton(const char *filename)
{
    ifstream infile(filename);
    float v1, v2, v3;
    int num;
    while (infile >> v1 >> v2 >> v3 >> num)
    {
        Joint *joint = new Joint;
        joint->transform = glm::translate(glm::mat4(1.0f), glm::vec3(v1, v2, v3));

        m_joints.push_back(joint);

        if (num == -1)
        {
            m_rootJoint = joint;
        }
        else
        {
            m_joints.at(num)->children.push_back(joint);
        }
    }
}

///=========================================================================================///
///                         Compute transformations for Joints and Bones
///=========================================================================================///

void SkeletalModel::computeTransforms()
{
    if (m_joints.size() == 0)
        return;

    computeJointTransforms();

    computeBoneTransforms();
}

// Compute a transformation matrix for each joint (i.e., ball) of the skeleton
void SkeletalModel::computeJointTransforms()
{
    jointMatList.clear();

    m_matrixStack.clear();

    computeJointTransforms(m_rootJoint, m_matrixStack);
}

// TODO: You will need to implement this recursive helper function to traverse the joint hierarchy for computing transformations of the joints
void SkeletalModel::computeJointTransforms(Joint *joint, MatrixStack matrix_stack)
{
    matrix_stack.push(glm::transpose(joint->transform));

    for (auto child : joint->children)
    {
        computeJointTransforms(child, matrix_stack);
    }
    cout << glm::to_string(matrix_stack.top()) << endl;
    jointMatList.push_back(matrix_stack.top());
    matrix_stack.pop();
}

// Compute a transformation matrix for each bone (i.e., cylinder) between each pair of joints in the skeleton
void SkeletalModel::computeBoneTransforms()
{
    boneMatList.clear();

    m_matrixStack.clear();

    computeBoneTransforms(m_rootJoint, m_matrixStack);
}

// TODO: You will need to implement this recursive helper function to traverse the joint hierarchy for computing transformations of the bones
void SkeletalModel::computeBoneTransforms(Joint *joint, MatrixStack matrixStack)
{
    matrixStack.push(transpose(joint->transform));

    for (auto child : joint->children)
    {
        glm::vec3 direction = child->transform[3];

        glm::vec3 z_axis = glm::normalize(direction);
        glm::vec3 y_axis = glm::normalize(glm::cross(z_axis, glm::vec3(0, 0, 1)));
        glm::vec3 x_axis = glm::normalize(glm::cross(y_axis, z_axis));

        glm::mat3 rotation(x_axis, y_axis, z_axis);
        glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.5));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, glm::length(direction)));
        glm::mat4 R = glm::mat4(rotation);

        boneMatList.push_back(transpose(R * S * T) * matrixStack.top());

        computeBoneTransforms(child, matrixStack);
    }
    matrixStack.pop();
}

///=========================================================================================///
///                              Set Joint Angles for Transform
///=========================================================================================///

// TODO: Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
void SkeletalModel::setJointTransform(int joint_index, float angle_x, float angle_y, float angle_z)
{
    glm::mat4 mx, my, mz;
    glm::mat4 identity_matrix = glm::mat4(1.0f);

    mx = glm::rotate(identity_matrix, angle_x, glm::vec3(1, 0, 0));
    my = glm::rotate(identity_matrix, angle_y, glm::vec3(0, 1, 0));
    mz = glm::rotate(identity_matrix, angle_z, glm::vec3(0, 0, 1));

    Joint *joint = m_joints.at(joint_index);
    joint->transform = joint->transform * mx * my * mz;
}
