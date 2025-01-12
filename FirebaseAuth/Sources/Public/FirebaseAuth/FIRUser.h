/*
 * Copyright 2017 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import <Foundation/Foundation.h>

#import "FIRAuth.h"
#import "FIRAuthDataResult.h"
#import "FIRMultiFactor.h"
#import "FIRUserInfo.h"

@class FIRAuthTokenResult;
@class FIRPhoneAuthCredential;
@class FIRUserProfileChangeRequest;
@class FIRUserMetadata;
@protocol FIRAuthUIDelegate;

NS_ASSUME_NONNULL_BEGIN

/** @typedef FIRAuthTokenCallback
    @brief The type of block called when a token is ready for use.
    @see `User.getIDToken()`
    @see `User.idTokenForcingRefresh(_:)`

    @param token Optionally; an access token if the request was successful.
    @param error Optionally; the error which occurred - or nil if the request was successful.

    @remarks One of `token` or `error` will always be non-nil.
 */
typedef void (^FIRAuthTokenCallback)(NSString *_Nullable token, NSError *_Nullable error)
    NS_SWIFT_UNAVAILABLE("Use Swift's closure syntax instead.");

/** @typedef FIRAuthTokenResultCallback
    @brief The type of block called when a token is ready for use.
    @see `User.getIDToken()`
    @see `User.idTokenForcingRefresh(_:)`

    @param tokenResult Optionally; an object containing the raw access token string as well as other
        useful data pertaining to the token.
    @param error Optionally; the error which occurred - or nil if the request was successful.

    @remarks One of `token` or `error` will always be non-nil.
 */
typedef void (^FIRAuthTokenResultCallback)(FIRAuthTokenResult *_Nullable tokenResult,
                                           NSError *_Nullable error)
    NS_SWIFT_UNAVAILABLE("Use Swift's closure syntax instead.");

/** @typedef FIRUserProfileChangeCallback
    @brief The type of block called when a user profile change has finished.

    @param error Optionally; the error which occurred - or nil if the request was successful.
 */
typedef void (^FIRUserProfileChangeCallback)(NSError *_Nullable error)
    NS_SWIFT_UNAVAILABLE("Use Swift's closure syntax instead.");

/** @typedef FIRSendEmailVerificationCallback
    @brief The type of block called when a request to send an email verification has finished.

    @param error Optionally; the error which occurred - or nil if the request was successful.
 */
typedef void (^FIRSendEmailVerificationCallback)(NSError *_Nullable error)
    NS_SWIFT_UNAVAILABLE("Use Swift's closure syntax instead.");

/** @class FIRUser
    @brief Represents a user. Firebase Auth does not attempt to validate users
        when loading them from the keychain. Invalidated users (such as those
        whose passwords have been changed on another client) are automatically
        logged out when an auth-dependent operation is attempted or when the
        ID token is automatically refreshed.
    @remarks This class is thread-safe.
 */
NS_SWIFT_NAME(User)
@interface FIRUser : NSObject <FIRUserInfo>

/** @property anonymous
    @brief Indicates the user represents an anonymous user.
 */
@property(nonatomic, readonly, getter=isAnonymous) BOOL anonymous;

/** @property emailVerified
    @brief Indicates the email address associated with this user has been verified.
 */
@property(nonatomic, readonly, getter=isEmailVerified) BOOL emailVerified;

/** @property refreshToken
    @brief A refresh token; useful for obtaining new access tokens independently.
    @remarks This property should only be used for advanced scenarios, and is not typically needed.
 */
@property(nonatomic, readonly, nullable) NSString *refreshToken;

/** @property providerData
    @brief Profile data for each identity provider, if any.
    @remarks This data is cached on sign-in and updated when linking or unlinking.
 */
@property(nonatomic, readonly, nonnull) NSArray<id<FIRUserInfo>> *providerData;

/** @property metadata
    @brief Metadata associated with the Firebase user in question.
 */
@property(nonatomic, readonly, nonnull) FIRUserMetadata *metadata;

/** @property tenantID
    @brief The tenant ID of the current user. nil if none is available.
 */
@property(nonatomic, readonly, nullable) NSString *tenantID;

/** @property multiFactor
    @brief Multi factor object associated with the user.
        This property is available on iOS only.
*/
@property(nonatomic, readonly, nonnull)
    FIRMultiFactor *multiFactor API_UNAVAILABLE(macos, tvos, watchos);

/** @fn init
    @brief This class should not be instantiated.
    @remarks To retrieve the current user, use `Auth.currentUser`. To sign a user
        in or out, use the methods on `Auth`.
 */
- (instancetype)init NS_UNAVAILABLE;

/** @fn updateEmail:completion:
    @brief Updates the email address for the user. On success, the cached user profile data is
   updated. Throws FIRAuthErrorCodeInvalidCredentials error when [Email Enumeration
   Protection](https://cloud.google.com/identity-platform/docs/admin/email-enumeration-protection)
   is enabled.
    @remarks May fail if there is already an account with this email address that was created using
        email and password authentication.

    @param email The email address for the user.
    @param completion Optionally; the block invoked when the user profile change has finished.
        Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeInvalidRecipientEmail` - Indicates an invalid recipient email was
            sent in the request.
        + `AuthErrorCodeInvalidSender` - Indicates an invalid sender email is set in
            the console for this action.
        + `AuthErrorCodeInvalidMessagePayload` - Indicates an invalid email template for
            sending update email.
        + `AuthErrorCodeEmailAlreadyInUse` - Indicates the email is already in use by another
            account.
        + `AuthErrorCodeInvalidEmail` - Indicates the email address is malformed.
        + `AuthErrorCodeRequiresRecentLogin` - Updating a user’s email is a security
            sensitive operation that requires a recent login from the user. This error indicates
            the user has not signed in recently enough. To resolve, reauthenticate the user by
            calling `reauthenticate(with:)`.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.
 */
- (void)updateEmail:(NSString *)email
         completion:(nullable void (^)(NSError *_Nullable error))completion
    NS_SWIFT_NAME(updateEmail(to:completion:)) DEPRECATED_MSG_ATTRIBUTE(
        "This method is deprecated and will be removed in a future release. Use "
        "sendEmailVerification(beforeUpdatingEmail email: String) instead.");

/** @fn updatePassword:completion:
    @brief Updates the password for the user. On success, the cached user profile data is updated.

    @param password The new password for the user.
    @param completion Optionally; the block invoked when the user profile change has finished.
        Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeOperationNotAllowed` - Indicates the administrator disabled
            sign in with the specified identity provider.
        + `AuthErrorCodeRequiresRecentLogin` - Updating a user’s password is a security
            sensitive operation that requires a recent login from the user. This error indicates
            the user has not signed in recently enough. To resolve, reauthenticate the user by
            calling `reauthenticate(with:)`.
        + `AuthErrorCodeWeakPassword` - Indicates an attempt to set a password that is
            considered too weak. The `NSLocalizedFailureReasonErrorKey` field in the `userInfo`
            dictionary object will contain more detailed explanation that can be shown to the user.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.
 */
- (void)updatePassword:(NSString *)password
            completion:(nullable void (^)(NSError *_Nullable error))completion
    NS_SWIFT_NAME(updatePassword(to:completion:));

/** @fn updatePhoneNumberCredential:completion:
    @brief Updates the phone number for the user. On success, the cached user profile data is
        updated.
        This method is available on iOS only.

    @param phoneNumberCredential The new phone number credential corresponding to the phone number
        to be added to the Firebase account, if a phone number is already linked to the account this
        new phone number will replace it.
    @param completion Optionally; the block invoked when the user profile change has finished.
        Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeRequiresRecentLogin` - Updating a user’s phone number is a security
            sensitive operation that requires a recent login from the user. This error indicates
            the user has not signed in recently enough. To resolve, reauthenticate the user by
            calling `reauthenticate(with:)`.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.
 */
- (void)updatePhoneNumberCredential:(FIRPhoneAuthCredential *)phoneNumberCredential
                         completion:(nullable void (^)(NSError *_Nullable error))completion
    API_UNAVAILABLE(macos, tvos, watchos);

/** @fn profileChangeRequest
    @brief Creates an object which may be used to change the user's profile data.

    @remarks Set the properties of the returned object, then call
        `UserProfileChangeRequest.commitChanges()` to perform the updates atomically.

    @return An object which may be used to change the user's profile data atomically.
 */
- (FIRUserProfileChangeRequest *)profileChangeRequest NS_SWIFT_NAME(createProfileChangeRequest());

/** @fn reloadWithCompletion:
    @brief Reloads the user's profile data from the server.

    @param completion Optionally; the block invoked when the reload has finished. Invoked
        asynchronously on the main thread in the future.

    @remarks May fail with a `AuthErrorCodeRequiresRecentLogin` error code. In this case
        you should call `reauthenticate(with:)` before re-invoking
        `updateEmail(to:)`.

    @remarks See `AuthErrors` for a list of error codes that are common to all API methods.
 */
- (void)reloadWithCompletion:(nullable void (^)(NSError *_Nullable error))completion;

/** @fn reauthenticateWithCredential:completion:
    @brief Renews the user's authentication tokens by validating a fresh set of credentials supplied
        by the user  and returns additional identity provider data.

    @param credential A user-supplied credential, which will be validated by the server. This can be
        a successful third-party identity provider sign-in, or an email address and password.
    @param completion Optionally; the block invoked when the re-authentication operation has
        finished. Invoked asynchronously on the main thread in the future.

    @remarks If the user associated with the supplied credential is different from the current user,
        or if the validation of the supplied credentials fails; an error is returned and the current
        user remains signed in.

    @remarks Possible error codes:

        + `AuthErrorCodeInvalidCredential` - Indicates the supplied credential is invalid.
            This could happen if it has expired or it is malformed.
        + `AuthErrorCodeOperationNotAllowed` - Indicates that accounts with the
            identity provider represented by the credential are not enabled. Enable them in the
            Auth section of the Firebase console.
        + `AuthErrorCodeEmailAlreadyInUse` -  Indicates the email asserted by the credential
            (e.g. the email in a Facebook access token) is already in use by an existing account,
            that cannot be authenticated with this method. Call `Auth.fetchSignInMethods(forEmail:)`
            for this user’s email and then prompt them to sign in with any of the sign-in providers
            returned. This error will only be thrown if the "One account per email address"
            setting is enabled in the Firebase console, under Auth settings. Please note that the
            error code raised in this specific situation may not be the same on Web and Android.
        + `AuthErrorCodeUserDisabled` - Indicates the user's account is disabled.
        + `AuthErrorCodeWrongPassword` - Indicates the user attempted reauthentication with
            an incorrect password, if credential is of the type `EmailPasswordAuthCredential`.
        + `AuthErrorCodeUserMismatch` -  Indicates that an attempt was made to
            reauthenticate with a user which is not the current user.
        + `AuthErrorCodeInvalidEmail` - Indicates the email address is malformed.

    @remarks See `FIRAuthErrors` for a list of error codes that are common to all API methods.
 */
- (void)reauthenticateWithCredential:(FIRAuthCredential *)credential
                          completion:(nullable void (^)(FIRAuthDataResult *_Nullable authResult,
                                                        NSError *_Nullable error))completion;

/** @fn reauthenticateWithProvider:UIDelegate:completion:
    @brief Renews the user's authentication using the provided auth provider instance.
        This method is available on iOS, macOS Catalyst, and tvOS only.

    @param provider An instance of an auth provider used to initiate the reauthenticate flow.
    @param UIDelegate Optionally an instance of a class conforming to the `AuthUIDelegate`
        protocol, used for presenting the web context. If nil, a default `AuthUIDelegate`
        will be used.
    @param completion Optionally; a block which is invoked when the reauthenticate flow finishes, or
        is canceled. Invoked asynchronously on the main thread in the future.
 */
- (void)reauthenticateWithProvider:(id<FIRFederatedAuthProvider>)provider
                        UIDelegate:(nullable id<FIRAuthUIDelegate>)UIDelegate
                        completion:(nullable void (^)(FIRAuthDataResult *_Nullable authResult,
                                                      NSError *_Nullable error))completion
    NS_SWIFT_NAME(reauthenticate(with:uiDelegate:completion:))API_UNAVAILABLE(macosx, watchos);

/** @fn getIDTokenResultWithCompletion:
    @brief Retrieves the Firebase authentication token, possibly refreshing it if it has expired.

    @param completion Optionally; the block invoked when the token is available. Invoked
        asynchronously on the main thread in the future.

    @remarks See `AuthErrors` for a list of error codes that are common to all API methods.
 */
- (void)getIDTokenResultWithCompletion:(nullable void (^)(FIRAuthTokenResult *_Nullable tokenResult,
                                                          NSError *_Nullable error))completion
    NS_SWIFT_NAME(getIDTokenResult(completion:));

/** @fn getIDTokenResultForcingRefresh:completion:
    @brief Retrieves the Firebase authentication token, possibly refreshing it if it has expired.

    @param forceRefresh Forces a token refresh. Useful if the token becomes invalid for some reason
        other than an expiration.
    @param completion Optionally; the block invoked when the token is available. Invoked
        asynchronously on the main thread in the future.

    @remarks The authentication token will be refreshed (by making a network request) if it has
        expired, or if `forceRefresh` is YES.

    @remarks See `AuthErrors` for a list of error codes that are common to all API methods.
 */
- (void)getIDTokenResultForcingRefresh:(BOOL)forceRefresh
                            completion:(nullable void (^)(FIRAuthTokenResult *_Nullable tokenResult,
                                                          NSError *_Nullable error))completion
    NS_SWIFT_NAME(getIDTokenResult(forcingRefresh:completion:));

/** @fn getIDTokenWithCompletion:
    @brief Retrieves the Firebase authentication token, possibly refreshing it if it has expired.

    @param completion Optionally; the block invoked when the token is available. Invoked
        asynchronously on the main thread in the future.

    @remarks See `AuthErrors` for a list of error codes that are common to all API methods.
 */
- (void)getIDTokenWithCompletion:
    (nullable void (^)(NSString *_Nullable token, NSError *_Nullable error))completion
    NS_SWIFT_NAME(getIDToken(completion:));

/** @fn getIDTokenForcingRefresh:completion:
    @brief Retrieves the Firebase authentication token, possibly refreshing it if it has expired.

    @param forceRefresh Forces a token refresh. Useful if the token becomes invalid for some reason
        other than an expiration.
    @param completion Optionally; the block invoked when the token is available. Invoked
        asynchronously on the main thread in the future.

    @remarks The authentication token will be refreshed (by making a network request) if it has
        expired, or if `forceRefresh` is true.

    @remarks See `AuthErrors` for a list of error codes that are common to all API methods.
 */
- (void)getIDTokenForcingRefresh:(BOOL)forceRefresh
                      completion:(nullable void (^)(NSString *_Nullable token,
                                                    NSError *_Nullable error))completion;

/** @fn linkWithCredential:completion:
    @brief Associates a user account from a third-party identity provider with this user and
        returns additional identity provider data.

    @param credential The credential for the identity provider.
    @param completion Optionally; the block invoked when the unlinking is complete, or fails.
        Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeProviderAlreadyLinked` - Indicates an attempt to link a provider of a
            type already linked to this account.
        + `AuthErrorCodeCredentialAlreadyInUse` - Indicates an attempt to link with a
            credential that has already been linked with a different Firebase account.
        + `AuthErrorCodeOperationNotAllowed` - Indicates that accounts with the identity
            provider represented by the credential are not enabled. Enable them in the Auth section
            of the Firebase console.

    @remarks This method may also return error codes associated with `updateEmail(to:)` and
        `updatePassword(to:)` on `User`.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.
 */
- (void)linkWithCredential:(FIRAuthCredential *)credential
                completion:(nullable void (^)(FIRAuthDataResult *_Nullable authResult,
                                              NSError *_Nullable error))completion;

/** @fn linkWithProvider:UIDelegate:completion:
    @brief link the user with the provided auth provider instance.
        This method is available on iOS, macOS Catalyst, and tvOS only.

    @param provider An instance of an auth provider used to initiate the link flow.
    @param UIDelegate Optionally an instance of a class conforming to the `AuthUIDelegate`
        protocol used for presenting the web context. If nil, a default `AuthUIDelegate`
        will be used.
    @param completion Optionally; a block which is invoked when the link flow finishes, or
        is canceled. Invoked asynchronously on the main thread in the future.
 */
- (void)linkWithProvider:(id<FIRFederatedAuthProvider>)provider
              UIDelegate:(nullable id<FIRAuthUIDelegate>)UIDelegate
              completion:(nullable void (^)(FIRAuthDataResult *_Nullable authResult,
                                            NSError *_Nullable error))completion
    NS_SWIFT_NAME(link(with:uiDelegate:completion:))API_UNAVAILABLE(macosx, watchos);

/** @fn unlinkFromProvider:completion:
    @brief Disassociates a user account from a third-party identity provider with this user.

    @param provider The provider ID of the provider to unlink.
    @param completion Optionally; the block invoked when the unlinking is complete, or fails.
        Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeNoSuchProvider` - Indicates an attempt to unlink a provider
            that is not linked to the account.
        + `AuthErrorCodeRequiresRecentLogin` - Updating email is a security sensitive
            operation that requires a recent login from the user. This error indicates the user
            has not signed in recently enough. To resolve, reauthenticate the user by calling
            `reauthenticate(with:)`.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.
 */
- (void)unlinkFromProvider:(NSString *)provider
                completion:(nullable void (^)(FIRUser *_Nullable user,
                                              NSError *_Nullable error))completion;

/** @fn sendEmailVerificationWithCompletion:
    @brief Initiates email verification for the user.

    @param completion Optionally; the block invoked when the request to send an email verification
        is complete, or fails. Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeInvalidRecipientEmail` - Indicates an invalid recipient email was
            sent in the request.
        + `AuthErrorCodeInvalidSender` - Indicates an invalid sender email is set in
            the console for this action.
        + `AuthErrorCodeInvalidMessagePayload` - Indicates an invalid email template for
            sending update email.
        + `AuthErrorCodeUserNotFound` - Indicates the user account was not found.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.
 */
- (void)sendEmailVerificationWithCompletion:(nullable void (^)(NSError *_Nullable error))completion;

/** @fn sendEmailVerificationWithActionCodeSettings:completion:
    @brief Initiates email verification for the user.

    @param actionCodeSettings An `ActionCodeSettings` object containing settings related to
        handling action codes.

    @remarks Possible error codes:

        + `AuthErrorCodeInvalidRecipientEmail` - Indicates an invalid recipient email was
            sent in the request.
        + `AuthErrorCodeInvalidSender` - Indicates an invalid sender email is set in
            the console for this action.
        + `AuthErrorCodeInvalidMessagePayload` - Indicates an invalid email template for
            sending update email.
        + `AuthErrorCodeUserNotFound` - Indicates the user account was not found.
        + `AuthErrorCodeMissingIosBundleID` - Indicates that the iOS bundle ID is missing when
            a iOS App Store ID is provided.
        + `AuthErrorCodeMissingAndroidPackageName` - Indicates that the android package name
            is missing when the `androidInstallApp` flag is set to true.
        + `AuthErrorCodeUnauthorizedDomain` - Indicates that the domain specified in the
            continue URL is not allowlisted in the Firebase console.
        + `AuthErrorCodeInvalidContinueURI` - Indicates that the domain specified in the
            continue URL is not valid.
 */
- (void)sendEmailVerificationWithActionCodeSettings:(FIRActionCodeSettings *)actionCodeSettings
                                         completion:(nullable void (^)(NSError *_Nullable error))
                                                        completion;

/** @fn deleteWithCompletion:
    @brief Deletes the user account (also signs out the user, if this was the current user).

    @param completion Optionally; the block invoked when the request to delete the account is
        complete, or fails. Invoked asynchronously on the main thread in the future.

    @remarks Possible error codes:

        + `AuthErrorCodeRequiresRecentLogin` - Updating email is a security sensitive
            operation that requires a recent login from the user. This error indicates the user
            has not signed in recently enough. To resolve, reauthenticate the user by calling
            `reauthenticate(with:)`.

    @remarks See `AuthErrors` for a list of error codes that are common to all `User` methods.

 */
- (void)deleteWithCompletion:(nullable void (^)(NSError *_Nullable error))completion;

/** @fn sendEmailVerificationBeforeUpdatingEmail:completion:
    @brief Send an email to verify the ownership of the account then update to the new email.
    @param email The email to be updated to.
    @param completion Optionally; the block invoked when the request to send the verification
        email is complete, or fails.
*/
- (void)sendEmailVerificationBeforeUpdatingEmail:(nonnull NSString *)email
                                      completion:
                                          (nullable void (^)(NSError *_Nullable error))completion;

/** @fn sendEmailVerificationBeforeUpdatingEmail:completion:
    @brief Send an email to verify the ownership of the account then update to the new email.
    @param email The email to be updated to.
    @param actionCodeSettings An `ActionCodeSettings` object containing settings related to
        handling action codes.
    @param completion Optionally; the block invoked when the request to send the verification
        email is complete, or fails.
*/
- (void)sendEmailVerificationBeforeUpdatingEmail:(nonnull NSString *)email
                              actionCodeSettings:(nonnull FIRActionCodeSettings *)actionCodeSettings
                                      completion:
                                          (nullable void (^)(NSError *_Nullable error))completion;

@end

/** @class FIRUserProfileChangeRequest
    @brief Represents an object capable of updating a user's profile data.
    @remarks Properties are marked as being part of a profile update when they are set. Setting a
        property value to nil is not the same as leaving the property unassigned.
 */
NS_SWIFT_NAME(UserProfileChangeRequest)
@interface FIRUserProfileChangeRequest : NSObject

/** @fn init
    @brief Please use `User.createProfileChangeRequest()` instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/** @property displayName
    @brief The user's display name.
    @remarks It is an error to set this property after calling
        `commitChanges()`.
 */
@property(nonatomic, copy, nullable) NSString *displayName;

/** @property photoURL
    @brief The user's photo URL.
    @remarks It is an error to set this property after calling
        `commitChanges()`.
 */
@property(nonatomic, copy, nullable) NSURL *photoURL;

/** @fn commitChangesWithCompletion:
    @brief Commits any pending changes.
    @remarks This method should only be called once. Once called, property values should not be
        changed.

    @param completion Optionally; the block invoked when the user profile change has been applied.
        Invoked asynchronously on the main thread in the future.
 */
- (void)commitChangesWithCompletion:(nullable void (^)(NSError *_Nullable error))completion;

@end

NS_ASSUME_NONNULL_END
