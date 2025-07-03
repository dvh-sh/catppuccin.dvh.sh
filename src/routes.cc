/*
 * @file routes.cc
 * @author David @dvhsh (https://dvh.sh)
 *
 * @created Thu, Jul 3, 2025
 * @updated Thu, Jul 3, 2025
 *
 * @description
 * This file implements the `setupRoutes` method of the CatppuccinAPI class,
 * defining all the HTTP endpoints for the API server and configuring
 * middleware such as CORS and rate limiting.
 */
#include "catppuccin_api.hpp"
#include <iostream>

namespace Catppuccin
{
    /*
     * @brief Sets up all API routes on the provided httplib server instance.
     * @param {httplib::Server&} server - The httplib server to configure.
     */
    void CatppuccinAPI::setupRoutes(httplib::Server &server)
    {
        server.set_mount_point("/", "./public");

        // CORS and rate limiting for all routes
        server.set_pre_routing_handler([this](const httplib::Request &req, httplib::Response &res)
                                       {
            // Add CORS headers
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            
            // Skip rate limiting for static files and OPTIONS requests
            if (req.method == "OPTIONS" || req.path == "/" || req.path.find('.') != std::string::npos) {
                return httplib::Server::HandlerResponse::Unhandled;
            }
            
            // Check rate limit for API endpoints
            if (!checkRateLimit(req, res)) {
                addRateLimitHeaders(req, res);
                return httplib::Server::HandlerResponse::Handled;
            }
            
            // Add rate limit headers to successful requests
            addRateLimitHeaders(req, res);
            return httplib::Server::HandlerResponse::Unhandled; });

        // GET /ports
        server.Get("/ports", [this](const httplib::Request &req, httplib::Response &res)
                   {
            int page = req.has_param("page") ? std::atoi(req.get_param_value("page").c_str()) : 1;
            int per_page = req.has_param("per_page") ? std::atoi(req.get_param_value("per_page").c_str()) : 20;
            
            ApiResponse response = getPorts(page, per_page);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 500;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /ports/:identifier
        server.Get("/ports/(.*)", [this](const httplib::Request &req, httplib::Response &res)
                   {
            std::string identifier = req.matches[1];
            ApiResponse response = getPort(identifier);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 404;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /collaborators
        server.Get("/collaborators", [this](const httplib::Request &req, httplib::Response &res)
                   {
            int page = req.has_param("page") ? std::atoi(req.get_param_value("page").c_str()) : 1;
            int per_page = req.has_param("per_page") ? std::atoi(req.get_param_value("per_page").c_str()) : 20;
            
            ApiResponse response = getCollaborators(page, per_page);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 500;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /collaborators/:username
        server.Get("/collaborators/(.*)", [this](const httplib::Request &req, httplib::Response &res)
                   {
            std::string username = req.matches[1];
            ApiResponse response = getCollaborator(username);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 404;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /categories
        server.Get("/categories", [this](const httplib::Request &req, httplib::Response &res)
                   {
            int page = req.has_param("page") ? std::atoi(req.get_param_value("page").c_str()) : 1;
            int per_page = req.has_param("per_page") ? std::atoi(req.get_param_value("per_page").c_str()) : 20;
            
            ApiResponse response = getCategories(page, per_page);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 500;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /categories/:key
        server.Get("/categories/(.*)", [this](const httplib::Request &req, httplib::Response &res)
                   {
            std::string key = req.matches[1];
            ApiResponse response = getCategory(key);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 404;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /showcases
        server.Get("/showcases", [this](const httplib::Request &req, httplib::Response &res)
                   {
            int page = req.has_param("page") ? std::atoi(req.get_param_value("page").c_str()) : 1;
            int per_page = req.has_param("per_page") ? std::atoi(req.get_param_value("per_page").c_str()) : 20;
            
            ApiResponse response = getShowcases(page, per_page);
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 500;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /userstyles
        server.Get("/userstyles", [this](const httplib::Request &req, httplib::Response &res)
                   {
    int page = req.has_param("page") ? std::atoi(req.get_param_value("page").c_str()) : 1;
    int per_page = req.has_param("per_page") ? std::atoi(req.get_param_value("per_page").c_str()) : 20;
    
    ApiResponse response = getUserstyles(page, per_page);
    
    if (response.success) {
        res.set_content(response.data.dump(2), "application/json");
    } else {
        res.status = 500;
        res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
    } });

        // GET /userstyles/:identifier
        server.Get("/userstyles/(.*)", [this](const httplib::Request &req, httplib::Response &res)
                   {
    std::string identifier = req.matches[1];
    ApiResponse response = getUserstyle(identifier);
    
    if (response.success) {
        res.set_content(response.data.dump(2), "application/json");
    } else {
        res.status = 404;
        res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
    } });

        // GET /palette
        server.Get("/palette", [this](const httplib::Request &, httplib::Response &res)
                   {
            ApiResponse response = getPalette();
            
            if (response.success) {
                res.set_content(response.data.dump(2), "application/json");
            } else {
                res.status = 500;
                res.set_content(json::object({{"error", response.error_message}}).dump(), "application/json");
            } });

        // GET /health (no rate limiting)
        server.Get("/health", [](const httplib::Request &, httplib::Response &res)
                   { res.set_content(json::object({{"status", "ok"}}).dump(), "application/json"); });

        // GET /rate-limit-status
        server.Get("/rate-limit-status", [this](const httplib::Request &req, httplib::Response &res)
                   {
            std::string client_ip = req.remote_addr;
            RateLimitInfo info = rate_limiter_.getRateLimitInfo(client_ip);
            
            json response = {
                {"limit", info.limit},
                {"used", info.requests_made},
                {"remaining", info.requests_remaining},
                {"reset_in_seconds", info.reset_time.count()},
                {"client_ip", client_ip}
            };
            
            res.set_content(response.dump(2), "application/json"); });

        // GET /metrics
        server.Get("/metrics", [this](const httplib::Request &, httplib::Response &res)
                   {
                request_count_++;
            ApiResponse response = getMetrics();
            res.set_content(response.data.dump(2), "application/json"); });
    }

}
