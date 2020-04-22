# coding: utf-8
from __future__ import print_function, division
import roslib
import unittest
import numpy as np
import pyexotica as exo
from pyexotica.testing import random_state

PKG = 'exotica_examples'
roslib.load_manifest(PKG)  # This line is not needed with Catkin.


def check_state_cost_jacobian_at_t(problem, t):
    scene = problem.get_scene()
    ds = scene.get_dynamics_solver()

    x = random_state(scene)
    u = np.random.random((ds.nu,))

    if t == problem.T - 1:
        problem.update_terminal_state(x)
    else:
        problem.update(x, u, t)
    J_solver = problem.get_state_cost_jacobian(t).copy()
    J_numdiff = np.zeros_like(J_solver)
    assert J_numdiff.shape[0] == ds.ndx

    eps = 1e-6
    for i in range(ds.ndx):
        dx = np.zeros((ds.ndx,))
        dx[i] = eps / 2.0

        x_plus = ds.integrate(x, dx, 1.0)
        x_minus = ds.integrate(x, dx, -1.0)

        if t == problem.T - 1:
            problem.update_terminal_state(x_plus)
        else:
            problem.update(x_plus, u, t)
        cost_high = problem.get_state_cost(t)

        if t == problem.T - 1:
            problem.update_terminal_state(x_minus)
        else:
            problem.update(x_minus, u, t)
        cost_low = problem.get_state_cost(t)

        J_numdiff[i] = (cost_high - cost_low) / eps
    np.testing.assert_allclose(J_solver, J_numdiff, rtol=1e-5,
                               atol=1e-5, err_msg='StateCostJacobian does not match!')


def check_state_cost_hessian_at_t(problem, t):
    scene = problem.get_scene()
    ds = scene.get_dynamics_solver()

    x = random_state(scene)
    u = np.random.random((ds.nu,))

    if t == problem.T - 1:
        problem.update_terminal_state(x)
    else:
        problem.update(x, u, t)
    H_solver = problem.get_state_cost_hessian(t).copy()
    H_numdiff = np.zeros_like(H_solver)
    assert H_numdiff.shape[0] == ds.ndx and H_numdiff.shape[1] == ds.ndx

    eps = 1e-6
    for i in range(ds.ndx):
        dx = np.zeros((ds.ndx,))
        dx[i] = eps / 2.0

        x_plus = ds.integrate(x, dx, 1.0)
        x_minus = ds.integrate(x, dx, -1.0)

        if t == problem.T - 1:
            problem.update_terminal_state(x_plus)
        else:
            problem.update(x_plus, u, t)
        jacobian_plus = problem.get_state_cost_jacobian(t)

        if t == problem.T - 1:
            problem.update_terminal_state(x_minus)
        else:
            problem.update(x_minus, u, t)
        jacobian_minus = problem.get_state_cost_jacobian(t)

        H_numdiff[:,i] = (jacobian_plus - jacobian_minus) / eps
    np.testing.assert_allclose(H_solver, H_numdiff, rtol=1e-5,
                               atol=1e-5, err_msg='StateCostHessian does not match!')


def check_control_cost_jacobian_at_t(problem, t):
    scene = problem.get_scene()
    ds = scene.get_dynamics_solver()

    x = random_state(scene)
    u = np.random.random((ds.nu,))

    problem.update(x, u, t)
    J_solver = problem.get_control_cost_jacobian(t).copy()
    J_numdiff = np.zeros_like(J_solver)
    assert J_numdiff.shape[0] == ds.nu

    eps = 1e-6
    for i in range(ds.nu):
        u_plus = u.copy()
        u_plus[i] += eps / 2.0
        u_minus = u.copy()
        u_minus[i] -= eps / 2.0

        problem.update(x, u_plus, t)
        cost_high = problem.get_control_cost(t)

        problem.update(x, u_minus, t)
        cost_low = problem.get_control_cost(t)

        J_numdiff[i] = (cost_high - cost_low) / eps
    np.testing.assert_allclose(J_solver, J_numdiff, rtol=1e-5,
                               atol=1e-5, err_msg='ControlCostJacobian does not match!')


def check_control_cost_hessian_at_t(problem, t):
    scene = problem.get_scene()
    ds = scene.get_dynamics_solver()

    x = random_state(scene)
    u = np.random.random((ds.nu,))

    problem.update(x, u, t)
    H_solver = problem.get_control_cost_hessian(t).copy()
    H_numdiff = np.zeros_like(H_solver)
    assert H_numdiff.shape[0] == ds.nu and H_numdiff.shape[1] == ds.nu

    eps = 1e-6
    for i in range(ds.nu):
        u_plus = u.copy()
        u_plus[i] += eps / 2.0
        u_minus = u.copy()
        u_minus[i] -= eps / 2.0

        problem.update(x, u_plus, t)
        jacobian_plus = problem.get_control_cost_jacobian(t)

        problem.update(x, u_minus, t)
        jacobian_minus = problem.get_control_cost_jacobian(t)

        H_numdiff[:,i] = (jacobian_plus - jacobian_minus) / eps
    np.testing.assert_allclose(H_solver, H_numdiff, rtol=1e-5,
                               atol=1e-5, err_msg='ControlCostHessian does not match!')

###############################################################################

if __name__ == "__main__":
    # Load a problem
    configs = [
        '{exotica_examples}/resources/configs/dynamic_time_indexed/01_ilqr_cartpole.xml',
        '{exotica_examples}/resources/configs/dynamic_time_indexed/02_lwr_task_maps.xml',
        '{exotica_examples}/resources/configs/dynamic_time_indexed/03_ilqr_valkyrie.xml',
        #'{exotica_satellite_dynamics_solver}/resources/config.apollo.xml',
    ]

    for config in configs:
        print("Testing", config)
        _, problem_init = exo.Initializers.load_xml_full(config)
        problem = exo.Setup.create_problem(problem_init)
        scene = problem.get_scene()
        ds = problem.get_scene().get_dynamics_solver()

        # print(problem, ds)
        # print(ds.nq, ds.nv, ds.nx, ds.ndx)

        # test state cost jacobian
        for t in range(problem.T):
            check_state_cost_jacobian_at_t(problem, t)

        # TODO: test state cost hessian
        for t in range(problem.T):
            check_state_cost_hessian_at_t(problem, t)

        # test control cost jacobian
        for t in range(problem.T - 1):
            check_control_cost_jacobian_at_t(problem, t)

        # test control cost hessian
        for t in range(problem.T - 1):
            check_control_cost_hessian_at_t(problem, t)

        # TODO: test state control cost hessian
