#pragma once

#include "goreecloud/browser/http_download_transport.hpp"

namespace goreecloud::browser {

class CurlHttpDownloadClient final : public HttpDownloadClient {
 public:
  CurlHttpDownloadClient();
  ~CurlHttpDownloadClient() override;

  std::optional<HttpDownloadResponseMetadata> inspect(
      const HttpDownloadRequest& request) override;
  HttpDownloadStreamResult stream(const HttpDownloadRequest& request,
                                  ChunkCallback on_chunk) override;
};

}  // namespace goreecloud::browser
